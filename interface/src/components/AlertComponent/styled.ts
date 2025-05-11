import styled from 'styled-components';

export const AlertContainer = styled.div`
    position: fixed;
    top: 20px;
    left: 50%;
    transform: translateX(-50%);
    background-color: #f8d7da;
    color: #721c24;
    padding: 15px;
    border: 1px solid #f5c6cb;
    border-radius: 5px;
    z-index: 9999;
`;

export const CloseButton = styled.button`
    margin-left: 10px;
    background-color: transparent;
    border: none;
    color: #721c24;
    cursor: pointer;
    font-weight: bold;
`;
