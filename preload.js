const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('Backend', {
  setTitle: (title) => ipcRenderer.send('set-title', title),
  full_screen: (value) => ipcRenderer.send('full-screen', value),
  close_window: (value) => ipcRenderer.send('close-window', value),
  title_bar_overlay: (symbol_color , back_color) => ipcRenderer.send('title-bar-overlay', symbol_color , back_color),
  win_minimize: (value) => ipcRenderer.send('win-minimize', value),
  win_resizable: (value) => ipcRenderer.send('win-resizable' , value),
  win_center: () => ipcRenderer.send('win-center'),
  win_size: (height , width , animate) => ipcRenderer.send('win-size' , height , width , animate),
  win_icon : (ic_path) => ipcRenderer.send('win-icon' , ic_path),
  
  getCameraAccess: () => ipcRenderer.invoke('get-camera-access'),
  getMicrophoneAccess: () => ipcRenderer.invoke('get-microphone-access'),
  // experimental
  extension : async (value) => await ipcRenderer.invoke('extension' , value),
})
