document.getElementById('config-form').addEventListener('submit', async (e) => {
    e.preventDefault();
  
    const configData = {
      run: {
        run_name: document.getElementById('run_name').value,
        simulations: parseInt(document.getElementById('simulations').value)
      },
      material: {
        lambda: parseFloat(document.getElementById('lambda').value),
        pabs: parseFloat(document.getElementById('pabs').value),
        k: parseFloat(document.getElementById('k').value)
      },
      geometry: {
        shape: document.getElementById('shape').value
      }
    };
  
    await window.electronAPI.saveConfig(configData);
    alert('File saved.');
  });
  