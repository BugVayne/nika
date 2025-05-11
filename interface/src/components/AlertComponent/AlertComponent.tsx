import React from 'react';
import { AlertContainer, CloseButton } from './styled';

const Alert = ({ message, onClose }) => {
    return (
        <AlertContainer>
            {message}
            <CloseButton onClick={onClose}> </CloseButton>
        </AlertContainer>
    );
};

export default Alert;
