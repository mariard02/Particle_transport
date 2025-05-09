const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  saveConfig: (data) => ipcRenderer.invoke('save-config', data),
  runBashScript: () => ipcRenderer.invoke('run-bash-script')
});