const { app, BrowserWindow, ipcMain , Menu , session } = require('electron');
const path = require('path');
const fs = require('fs');


const options = {
    url: '',
    width: 800,
    height: 600,
    resizable : true ,
    title: true,
    title_text: 'EMWebKit',
    title_style: 'hiddenInset',
    title_symbol_color: '',
    title_bar_color: '',
    full_screen: false,
    center: true,
    posx: 0,
    posy: 0,
    strict_url: false,
    url_style: '',
    icon : '_8452d7c4-ac7a-42e0-9237-5858d1716314.jpeg'
};

function loadConfig(configPath) {
    try {
        const configContents = fs.readFileSync(configPath, 'utf8');
        const configOptions = JSON.parse(configContents);
        
        Object.assign(options, configOptions);
    } catch (error) {
        console.error('Error loading config file:', error);
    }
}

function createWindow() {
    const configIndex = process.argv.indexOf('-CONFIG');
    if (configIndex !== -1 && process.argv[configIndex + 1]) {
        loadConfig(process.argv[configIndex + 1]);
    }
    

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
        frame: options.title, 
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
    };
    

    
    if (options.center) {
        mainWindowOptions.center = true;
    } else {
        mainWindowOptions.x = options.posx;
        mainWindowOptions.y = options.posy;
    }

    const mainWindow = new BrowserWindow(mainWindowOptions);
    const isUrlAllowed = (newURL) => {
        return newURL.startsWith(options.url_style);
    };

    // if smth like native menu bar is needed create it with html , css and js
    const customMenuTemplate = [];
    const customMenu = Menu.buildFromTemplate(customMenuTemplate);
    Menu.setApplicationMenu(customMenu);

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
                mainWindow.loadURL(options.url_style); 
            }
        });

        mainWindow.webContents.on('new-window', (event, newURL) => {
            if (!isUrlAllowed(newURL)) {
                event.preventDefault();
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
}

app.whenReady().then(() => {
    createWindow();

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
});


app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        session.defaultSession.clearStorageData().then(() => {
            app.quit();
        });
    }else{
        session.defaultSession.clearStorageData()
    }
});
