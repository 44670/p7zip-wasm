// Worker for processing archive files

var wasmOutFileMap = {}

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
    this.write = function (u8Arr) {
        var tmpBuf = new Uint8Array(u8Arr.length);
        tmpBuf.set(u8Arr);
        this.bufs.push(tmpBuf);
    }
}

function wasmOnWrite(idx, ptr, len) {
    console.log('wasmOnWrite', idx, ptr, len);
    var f = wasmOutFileMap[idx]
    if (!f) {
        console.log('skip');
        return;
    }
    f.write(Module.HEAPU8.subarray(ptr, ptr + len));
}


function openArchvie(data) {
    var blob = data.blob
    blob.name = 'archive';
    console.log('worker openArchvie', blob);
    // Mount workerfs
    FS.mkdir('/worker');
    FS.mount(WORKERFS, {
        blobs: [{
            name: 'archive',
            data: blob
        }]
    }, '/worker');
    var ret = Module._apiOpenArchive();
    if (ret != 0) {
        self.postMessage({
            t: 'error',
            msg: 'Failed to open archive',
            ret: ret
        });
        return;
    }
    var bufLen = Module._apiListFiles();
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
            name: str,
            idx: idx
        })
        idx += 1
        pos += 7 + strLen;
    }

    console.log(fileList)

    self.postMessage({
        t: 'opened',
        fileList: fileList
    });
}






function extractOneFile(data) {
    var idx = data.idx
    var fout = new WasmOutFile();
    var name = data.name

    wasmOutFileMap[idx] = fout;
    var ret = Module._apiExtractOneFile(idx)
    if (ret != 0) {
        alert('Extract failed ' + ret);
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