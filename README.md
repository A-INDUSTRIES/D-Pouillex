# **D-Pouillex - BAB1 Project**
***
## *Hiya, this repository (will)contains the code for our first engineering year project @D-Pouillex.*
### This code is intended to be run on a ``Adafruit CRICKIT for Circuit Playground Express`` with the `adafruit_circuitplayground` python package.
### Package to be installed with :
```bash
pip install adafruit_circuitplayground
```
[See documentation on how to get started](https://learn.adafruit.com/welcome-to-circuitpython)
[See documentation of the package](https://learn.adafruit.com/circuitpython-made-easy-on-circuit-playground-express/first-things-first)
#### However, if you do not have an adafruit crickit for circuit playground express, you may use this extension (deprecated) for vscode.
[Vscode Python DeviceSimulator](https://github.com/microsoft/vscode-python-devicesimulator)

[Direct download link](https://github.com/microsoft/vscode-python-devicesimulator/releases/download/36321-staging/devicesimulatorexpress-2020.0.36321.vsix)
#### Were you to install it via VSIX, you would encounter a python path error on extension lauch.
#### Here's a quick and **dirty** fix *(for windows)*:
#### Go to `~/.vscode/extensions/ms-python.devicesimulatorexpress-2020.0.36321` (may differ if you are using another version)
#### Then open the file at `out/service/setupService.js`
#### You should see the following:
```js
const fs = require("fs");
const os = require("os");
const path = require("path");
const vscode = require("vscode");
const constants_1 = require("../constants");
const utils_1 = require("../extension_utils/utils");
class SetupService {
    constructor(telemetryAI) {
        this.setupEnv = (context, needsResponse = false) => __awaiter(this, void 0, void 0, function* () {
            const originalPythonExecutablePath = yield this.getCurrentPythonExecutablePath();
            if (originalPythonExecutablePath === "") {
                return;
            }
            let pythonExecutablePath = originalPythonExecutablePath;
```
#### Replace the above code with this:
```js
const fs = require("fs");
const os = require("os");
const path = require("path");
const vscode = require("vscode");
const constants_1 = require("../constants");
const utils_1 = require("../extension_utils/utils");
const originalPythonExecutablePath = "LITTERAL_PATH_TO_YOUR_PYTHON_EXECUTABLE"
class SetupService {
    constructor(telemetryAI) {
        this.setupEnv = (context, needsResponse = false) => __awaiter(this, void 0, void 0, function* () {
            let pythonExecutablePath = originalPythonExecutablePath;
```
#### Do not forget to replace `"LITTERAL_PATH_TO_YOUR_PYTHON_EXECUTABLE"` with your actual python path which may look like this : `C:\\Program Files\\Python37\\python.exe`

#### And you should be good to go!

## Contributing
### Any contribution is appreciated! :grin: