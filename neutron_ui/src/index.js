const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const fs = require('fs');
const { exec } = require('child_process');

if (require('electron-squirrel-startup')) {
  app.quit();
}

const createWindow = () => {
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
    },
  });

  mainWindow.loadFile(path.join(__dirname, 'index.html'));
  mainWindow.webContents.openDevTools();
};

app.whenReady().then(() => {
  createWindow();
  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', () => {
  app.quit();
});

ipcMain.handle('save-config', async (event, data) => {
  try {
  const filePath = path.join(__dirname,'..','..','..','..', '..', 'config_new.json');
  fs.writeFileSync(filePath, JSON.stringify(data, null, 2));
  } catch(error) {
      console.error('Error saving config:', error);
      return { success: false, error: error.message };

  }
});

ipcMain.handle('run-bash-script', async () => {
  return new Promise((resolve, reject) => {
    exec('bash /Users/maria/Desktop/Máster/M1/S2/Scientific_computing_and_software_design/Particle_transport/Particle_Transport.sh config.json', (error, stdout, stderr) => {
      if (error) {
        console.error(`Error ejecutando script: ${error.message}`);
        reject(error);
      } else {
        console.log(`stdout: ${stdout}`);
        console.error(`stderr: ${stderr}`);
        resolve(stdout);
      }
    });
  });
});