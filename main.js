const { app, BrowserWindow, ipcMain , Menu , session , shell , systemPreferences } = require('electron');
const path = require('path');
const fs = require('fs');

// just to make everything be clear
// in the future for each gui app , this data will be saved where 
// the app wants !
const appDirectory = path.dirname(app.getAppPath());
const customDirectoryPath = path.join(appDirectory, 'RenderCaches');
app.setPath('userData', customDirectoryPath);
app.setPath('appData', customDirectoryPath);
app.setPath('temp', customDirectoryPath);

const options = {
    url: '',
    width: 800,
    height: 600,
    resizable : true ,
    title: true, // Changed default to true
    title_text: 'EMWebKit',
    title_style: 'hiddenInset',
    title_symbol_color: '',
    title_bar_color: '',
    full_screen: false,
    center: true,
    posx: 0,
    posy: 0,
    strict_url: true,
    url_style: `file://${path.join(__dirname, 'welcome.html')}`,
    icon : 'icons/windows.ico'
};

const currentTime = new Date().getTime(); // Get the current timestamp
const customPartition = `my-session-${currentTime}`;
var custtomSession

function loadConfig(configPath) {

    if (fs.existsSync(configPath)){
        if (fs.statSync(configPath).isDirectory()) { 
            configPath = path.join(configPath, 'MUTEXIS_KIT.json'); 
        }
    
        try {
            const configContents = fs.readFileSync(configPath, 'utf8');
            const configOptions = JSON.parse(configContents);
        // Merge default options with the ones from the config file
            Object.assign(options, configOptions);
        } catch (error) {
            console.error('Error loading config file:', error);
        }
    }
}

function config_check() { 
    const filePath = path.join(process.cwd(), 'MUTEXIS_KIT.json'); // Check if the file exists in the current directory 
    if (fs.existsSync(filePath)) { 
        loadConfig(filePath);
    }
}

function createWindow() {
    const configIndex = process.argv.indexOf('-CONFIG');
    if (configIndex !== -1 && process.argv[configIndex + 1]) {
        loadConfig(process.argv[configIndex + 1]);
    }else{
        config_check()
    }

    // experimental
    // const extension_file = process.argv.indexOf('-EXTENSION');
    // if (extension_file !== -1 && process.argv[configIndex + 1]) {
        // ongoing project
        // process.argv[extension_file + 1]
    // }
    
    custtomSession = session.fromPartition(customPartition)
    const mainWindowOptions = {
        resizable : options.resizable ,
        autoHideMenuBar: true,
        icon : options.icon,
        width: options.width,
        height: options.height,
        fullscreen: options.full_screen,
        title: options.title_text,
        titleBarStyle: options.title_style,
        titleBarOverlay: {
            color: options.title_bar_color, 
            symbolColor: options.title_symbol_color,
        }, 
        frame: options.title, // Show or hide the title bar based on the title option
        webPreferences: {
            // if you want it be possible to change stuff in the engine backend , remove the line below
            // the nodeIntegration set to false , makes all dynamic changes from preload.js invalid
            // nodeIntegration: false ,
            session : custtomSession ,
            preload: path.join(__dirname, 'preload.js'),
            contextIsolation: true ,
            webviewTag: true ,
        }
    };
    

    
    if (options.center) {
        mainWindowOptions.center = true;
    } else {
        mainWindowOptions.x = options.posx;
        mainWindowOptions.y = options.posy;
    }

    const mainWindow = new BrowserWindow(mainWindowOptions);

    const customMenuTemplate = [];
    const customMenu = Menu.buildFromTemplate(customMenuTemplate);
    Menu.setApplicationMenu(customMenu);


    const isUrlAllowed = (newURL) => {
        return newURL.startsWith(options.url_style);
    };

    

    var urlToLoad;
    if (options.strict_url){
        if (isUrlAllowed(options.url)){
            urlToLoad = options.url || `file://${path.join(__dirname, 'welcome.html')}`;
        }else{
            urlToLoad = options.url_style || `file://${path.join(__dirname, 'welcome.html')}`;
        }
    }else{
        urlToLoad = options.url || `file://${path.join(__dirname, 'welcome.html')}`;
    }

    if (options.strict_url) {
        mainWindow.webContents.on('will-navigate', (event, newURL) => {
            if (!isUrlAllowed(newURL)) {
                event.preventDefault();
                shell.openExternal(newURL);
                mainWindow.loadURL(options.url_style);
            }
        });

        mainWindow.webContents.on('new-window', (event, newURL) => {
            if (!isUrlAllowed(newURL)) {
                event.preventDefault();
                shell.openExternal(newURL);
                mainWindow.loadURL(options.url_style);
            }
        });
    }
    
    mainWindow.loadURL(urlToLoad);
    mainWindow.removeMenu();

    if (options.title) {
        mainWindow.setTitle(options.title_text);
    }

    ipcMain.on('set-title', (event, title) => {
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        console.log(title)
        win.setTitle(title)
    })

    ipcMain.on('full-screen' , (event , value) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        console.log(value)
        if (value == 'true') {
           
            win.setFullScreen(true);
        } else {
          
            win.setFullScreen(false);
        }
    })
    
    ipcMain.on('close-window' , (event , value) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        if (value == 'true') {
            session.defaultSession.clearStorageData()
            custtomSession.clearStorageData()
            win.close();
        }
    })

    ipcMain.on('title-bar-overlay' , (event , symbol_color , back_color) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents);
        titleBarOverlay = {
            color: back_color, 
            symbolColor: symbol_color, 
        }
        win.setTitleBarOverlay(titleBarOverlay);
    })

    ipcMain.on('win-minimize' , (event , value) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        if (value == 'true') {
            win.minimize();
        }
    })

    ipcMain.on('win-resizable' , (event , value) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        if (value == 'true') {
            win.setResizable(true);
        }else{
            win.setResizable(false);
        }
    })

    ipcMain.on('win-center' , (event) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        win.center();
    })

    ipcMain.on('win-size' , (event , height , width , animate) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        if (animate == 'true'){
            win.setSize(width, height, true);
        }else{
            win.setSize(width, height, false);
        }
    })

    ipcMain.on('win-icon' , (event , ic_path) =>{
        const webContents = event.sender
        const win = BrowserWindow.fromWebContents(webContents)
        win.setIcon(ic_path);
    })

    ipcMain.handle('get-camera-access', async () => { 
        if (process.platform === 'darwin'){
            try { 
                const cameraPermission = await systemPreferences.askForMediaAccess('camera');
                return cameraPermission; // true if granted, false if denied 
            } catch (error) { 
                console.error('Error requesting camera permission:', error); 
                return false; // Return false if there is an error 
            } 
        }else{
            return true
        }
    });

    ipcMain.handle('get-microphone-access', async () => { 
        if (process.platform === 'darwin'){
            try { 
                const microphonePermission = await systemPreferences.askForMediaAccess('microphone'); 
                return microphonePermission; // true if granted, false if denied 
            } catch (error) { 
                console.error('Error requesting microphone permission:', error); 
                return false; // Return false if there is an error 
            } 
        }else{
            return true
        }
    });
}

app.whenReady().then(() => {
    const install_check = process.argv.indexOf('-v');

    if (install_check !== -1) {
        console.log('1.0.8');
        return;
    }


    createWindow();

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
});

// just to make sure
app.on('will-quit' , () => {
    custtomSession.clearStorageData()
    session.defaultSession.clearStorageData()
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        custtomSession.clearStorageData()
        session.defaultSession.clearStorageData().then(() => {
            app.quit();
        });
    }else{
        custtomSession.clearStorageData()
        session.defaultSession.clearStorageData()
    }
});
