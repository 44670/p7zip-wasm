// Worker for processing archive files
var state = {}
var wasmOutFileMap = {}
var outDirHandle = null
var archiveFileName = ''
var extractAllMode = false;

function postError(msg, ret) {
    self.postMessage({
        t: 'error',
        msg: msg,
        ret: ret
    });
}

function postDone(msg, ret) {
    self.postMessage({
        t: 'done',
        msg: msg,
        ret: ret
    });
}

function wasmReady() {
    // Send message to main page
    self.postMessage({
        t: 'ready'
    });
}

// Handle messages from main page
self.onmessage = function (e) {
    var data = e.data
    var type = data.t;
    var cmdMap = {
        'open': openArchvie,
        'extract': extractOneFile,
        'extractAll': extractAll
    }
    var fn = cmdMap[type];
    if (!fn) {
        console.log('Unknown command', type);
        return;
    }
    fn(data);
}


// Load the WASM module (build/7za.js)
importScripts('7za.js');



function WasmOutFile() {
    this.bufs = [];
    this.len = 0;
    this.state = 0;
    this.write = function (u8Arr) {
        var tmpBuf = new Uint8Array(u8Arr.length);
        tmpBuf.set(u8Arr);
        this.bufs.push(tmpBuf);
    }
}

async function wasmOnWrite(idx, ptr, len) {
    console.log('wasmOnWrite', idx, ptr, len);
    if (extractAllMode) {
        var fobj = state.fileList[idx];
        if (!fobj.fsWriteStream) {
            if (fobj.parent) {
                var dir = fobj.parent.fsDir; // Get the parent directory
                var fileHandle = await dir.getFileHandle(fobj.name, { create: true });
                var wirtableStream = await fileHandle.createWritable();
                fobj.fsWriteStream = wirtableStream;
            } else {
                console.log('!!! No parent for file', idx);
                return
            }
        }
        var writeStream = fobj.fsWriteStream;
        if (!writeStream) {
            console.log('!!! No stream for file', idx);
            return
        }
        var buf = new Uint8Array(Module.HEAPU8.buffer, ptr, len);
        await writeStream.write(buf);
        return
    }
    var f = wasmOutFileMap[idx]
    if (!f) {
        console.log('skip');
        return;
    }
    f.write(Module.HEAPU8.subarray(ptr, ptr + len));
}

async function wasmOnClose(idx) {
    console.log('wasmOnClose', idx);
    if (extractAllMode) {
        var writeStream = state.fileList[idx].fsWriteStream;
        if (writeStream) {
            await writeStream.close();
            state.fileList[idx].fsWriteStream = null;
        }
        return
    }
    var f = wasmOutFileMap[idx]
    if (f) {
        f.state = -1;
    }
}


async function openArchvie(data) {
    var blob = data.blob
    archiveFileName = blob.name;
    console.log('worker openArchvie', blob);
    // Mount workerfs
    FS.mkdir('/worker');
    FS.mount(WORKERFS, {
        blobs: [{
            name: 'archive',
            data: blob
        }]
    }, '/worker');
    var ret = await Module.ccall('apiOpenArchive', 'number', [], [], { async: true });
    if (ret != 0) {
        postError('Failed to open archive', ret);
        return;
    }
    var bufLen = await Module.ccall('apiListFiles', 'number', [], [], { async: true });
    console.log('wasmListBuf Len:', bufLen);
    var wasmListBuf = Module.HEAPU32.subarray(Module._getSymbol(0) / 4, 1 * 1024 * 1024);
    var fileList = []
    var str = ''
    var pos = 0;
    var idx = 0
    while (pos < bufLen) {
        if (wasmListBuf[pos] != 0xCAFEBABE) {
            console.log("Invalid magic number at " + pos);
            break;
        }
        var filetype = wasmListBuf[pos + 1];
        var fileSize = (wasmListBuf[pos + 3] << 32) | (wasmListBuf[pos + 2]);
        var fileTime = wasmListBuf[pos + 5];
        var strLen = wasmListBuf[pos + 6];
        var str = ''
        for (var i = 0; i < strLen; i++) {
            str += String.fromCharCode(wasmListBuf[pos + 7 + i]);
        }
        fileList.push({
            type: filetype,
            size: fileSize,
            time: fileTime,
            path: str,
            idx: idx
        })
        idx += 1
        pos += 7 + strLen;
    }

    console.log(fileList)
    state = {
        mode: 'open',
        fileList: fileList,
        currentDir: [],
        root: fileListToDirectory(fileList),
        archiveFileName: archiveFileName
    }

    self.postMessage({
        t: 'opened',
        state: state
    });
}






async function extractOneFile(data) {
    extractAllMode = false;

    var idx = data.idx
    var fout = new WasmOutFile();
    var name = data.name

    wasmOutFileMap[idx] = fout;
    var ret = await Module.ccall('apiExtractOneFile', 'number', ['number'], [idx], { async: true });
    console.log('wasmExtractOneFile', idx, ret);
    if (ret != 0) {
        postError('Failed to extract file', ret);
        return;
    }
    wasmOutFileMap[idx] = null;

    var blob = new Blob(fout.bufs, {
        type: 'application/octet-stream'
    });
    // Post message to main page
    self.postMessage({
        t: 'extracted',
        idx: idx,
        blob: blob,
        name: name
    });
}

async function ensureDirectory(root) {
    console.log('ensureDirectory', root.name);
    var dir = root.fsDir;
    for (var key in root.dirs) {
        root.dirs[key].fsDir = await dir.getDirectoryHandle(key, { create: true })
        await ensureDirectory(root.dirs[key]);
    }
}

async function extractAll(data) {
    extractAllMode = true;
    var targetDir = data.dir  // FileSystemDirectoryHandle
    var name = state.archiveFileName
    // Remove the file extension
    if (name.indexOf('.') >= 0) {
        name = name.substr(0, name.lastIndexOf('.'));
    }
    outDirHandle = await targetDir.getDirectoryHandle(name, { create: true });
    state.root.fsDir = outDirHandle;
    await ensureDirectory(state.root);
    var ret = await Module.ccall('apiExtractAll', 'number', [], [], { async: true });
    console.log('wasmExtractAll', ret);
    if (ret != 0) {
        postError('Failed to extract all files', ret);
        return;
    }
    postDone('Extracted all files', 0);
}


function fileListToDirectory(lst) {
    var ret = { type: 1, files: {}, dirs: {}, name: '/' }
    for (var i = 0; i < lst.length; i++) {
        var fobj = lst[i];
        var arr = fobj.path.split('/');
        var dir = ret;
        var isDir = fobj.type & 1;
        for (var j = 0; j < (isDir ? (arr.length) : (arr.length - 1)); j++) {
            var key = arr[j];
            if (!dir.dirs[key]) {
                dir.dirs[key] = { type: 1, files: {}, dirs: {}, name: key };
            }
            dir = dir.dirs[key];
        }
        if (!isDir) {
            fobj.parent = dir;
            fobj.name = arr[arr.length - 1];
            dir.files[arr[arr.length - 1]] = fobj;
        }
    }
    return ret;
}