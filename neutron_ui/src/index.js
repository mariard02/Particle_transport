const { app, BrowserWindow, ipcMain } = require('electron');

app.commandLine.appendSwitch('disable-logging');

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
      devTools: false
    },
  });

  mainWindow.loadFile(path.join(__dirname, 'index.html'));
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
  const filePath = path.join(__dirname,'..', '..', 'config_new.json');
  fs.writeFileSync(filePath, JSON.stringify(data, null, 2));
  } catch(error) {
      console.error('Error saving config:', error);
      return { success: false, error: error.message };

  }
});

ipcMain.handle('run-bash-script', async () => {
  return new Promise((resolve, reject) => {
    const scriptPath = path.join(__dirname, '..', '..', 'Particle_Transport.sh');
    
    if (!fs.existsSync(scriptPath)) {
      const error = new Error(`The script does not exist in: ${scriptPath}`);
      console.error(error.message);
      return reject(error);
    }

    if (process.platform !== 'win32') {
      try {
        fs.chmodSync(scriptPath, 0o755); 
      } catch (chmodError) {
        console.warn('The script permisions could not be changed', chmodError);
      }
    }

    const command = `bash "${scriptPath}" config.json`;
    console.log(`Executing: ${command}`);
    
    exec(command, { maxBuffer: 1024 * 1024 * 5 }, (error, stdout, stderr) => {
      if (error) {
        console.error(`Error executing script: ${error.message}`);
        console.error(`stderr: ${stderr}`);
        return reject(new Error(stderr || error.message));
      }
      console.log(`stdout: ${stdout}`);
      resolve(stdout);
    });
  });
});