#!/bin/bash

# Check if npm is installed
if ! command -v npm &> /dev/null
then
    echo "npm is not installed. Installing npm..."
    
    # Check if node is installed (npm comes with Node.js)
    if ! command -v node &> /dev/null
    then
        echo "Node.js is not installed. Installing Node.js and npm..."
        
        # Detect available package manager
        if command -v apt-get &> /dev/null
        then
            # Debian/Ubuntu based systems
            sudo apt-get update
            sudo apt-get install -y nodejs npm
        elif command -v yum &> /dev/null
        then
            # RedHat/CentOS based systems
            sudo yum install -y nodejs npm
        elif command -v brew &> /dev/null
        then
            # macOS with Homebrew
            brew install node
        else
            echo "Could not detect package manager. Please install Node.js manually."
            echo "Visit: https://nodejs.org/"
            exit 1
        fi
    else
        echo "Node.js is installed but npm is missing. This is unusual. Reinstalling Node.js to get npm..."
        # Reinstall Node.js to ensure npm is present
        if command -v apt-get &> /dev/null
        then
            sudo apt-get install --reinstall nodejs npm
        elif command -v yum &> /dev/null
        then
            sudo yum reinstall nodejs npm
        elif command -v brew &> /dev/null
        then
            brew reinstall node
        fi
    fi
    
    # Verify again after installation
    if ! command -v npm &> /dev/null
    then
        echo "npm installation failed. Please install it manually."
        exit 1
    fi
    
    echo "npm successfully installed."
fi

# Run npm start
echo "Starting the application..."

cd neutron_ui
npm start