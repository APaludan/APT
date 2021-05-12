const http = require('http');
const fs = require("fs");
const path = require("path");
const url = require("url");

const hostname = '127.0.0.1';
const port = 3000;
const rootFileSystem = process.cwd();
const server = http.createServer(requestHandler);

function requestHandler(req, res) {
    try {
        processReq(req, res);
    } catch (e) {
        console.log("Internal Error: " + e);
        errorResponse(res, 500, "");
    }
}

function processReq(req, res) {
    //console.log("GOT: " + req.method + " " + req.url);
    //https://www.w3schools.com/nodejs/nodejs_url.asp
    switch (req.method) {
        case "POST":
            switch (req.url) {
                case "/notspy":
                    // det virker sådan her: https://nodejs.dev/learn/get-http-request-body-data-using-nodejs
                    req.on('data', data => console.log("mouse on:", JSON.parse(data).mouseon));
                    res.statusCode = 200;
                    res.end();
                    break;
                default:
                    res.writeHead(500);
                    res.end();
            }
            break;
        case "GET":
            console.log(req.url);
            switch (req.url) {
                case "/":
                    fileResponse(res, "Rx/asd.html");
                    break;
                default:
                    fileResponse(res, `${req.url}`)
                    break;
            }
            break;
    }
}

/* start the server */
server.listen(port, hostname, () => {
    console.log(`Server running at http://${hostname}:${port}/`);
    //fs.writeFileSync('message.txt', `Server running at http://${hostname}:${port}/`);
});

/* send contents as file as response */
function fileResponse(res, filename) {
    const sPath = securePath(filename);
    console.log("Reading:" + sPath);
    fs.readFile(sPath, (err, data) => {
        if (err) {
            console.error(err);
            errorResponse(res, 404, String(err));
        } else {
            res.statusCode = 200;
            res.setHeader('Content-Type', guessMimeType(filename));
            res.write(data);
            res.end('\n');
        }
    })
}

function securePath(userPath) {
    if (userPath.indexOf('\0') !== -1) {
        // could also test for illegal chars: if (!/^[a-z0-9]+$/.test(filename)) {return undefined;}
        return undefined;
    }
    let p = path.join(rootFileSystem, path.normalize(userPath));
    //console.log("The path is:"+p);
    return p;
}

//A helper function that converts filename suffix to the corresponding HTTP content type
//better alternative: use require('mmmagic') library
function guessMimeType(fileName) {
    const fileExtension = fileName.split('.').pop().toLowerCase();
    console.log(fileExtension);
    const ext2Mime = { //Aught to check with IANA spec
        "txt": "text/txt",
        "html": "text/html",
        "ico": "image/ico", // CHECK x-icon vs image/vnd.microsoft.icon
        "js": "text/javascript",
        "json": "application/json",
        "css": 'text/css',
        "png": 'image/png',
        "jpg": 'image/jpeg',
        "wav": 'audio/wav',
        "mp3": 'audio/mpeg',
        "svg": 'image/svg+xml',
        "pdf": 'application/pdf',
        "doc": 'application/msword',
        "docx": 'application/msword'
    };
    //incomplete
    return (ext2Mime[fileExtension] || "text/plain");
}

/* send a response with a given HTTP error code, and reason string */
function errorResponse(res, code, reason) {
    res.statusCode = code;
    res.setHeader('Content-Type', 'text/txt');
    res.write(reason);
    res.end("\n");
}