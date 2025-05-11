// AlertContext.tsx
import React, { createContext, useContext, useState } from 'react';
import Alert from './AlertComponent';

interface AlertContextType {
    showAlert: (message: string) => void;
}

const AlertContext = createContext<AlertContextType | undefined>(undefined);

export const AlertProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
    const [alertMessage, setAlertMessage] = useState<string>('');
    const [isVisible, setIsVisible] = useState<boolean>(false);

    const showAlert = (message: string) => {
        setAlertMessage(message);
        setIsVisible(true);
        setTimeout(() => {
            setIsVisible(false);
            setAlertMessage('');
        }, 6000);
    };

    return (
        <AlertContext.Provider value={{ showAlert }}>
            {children}
            {isVisible && <Alert message={alertMessage} onClose={() => setIsVisible(false)} />}
        </AlertContext.Provider>
    );
};

export const useAlert = () => {
    const context = useContext(AlertContext);
    if (!context) {
        throw new Error('useAlert must be used within an AlertProvider');
    }
    return context;
};
