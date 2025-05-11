import styled from 'styled-components';

export const FullPageModalBackdrop = styled.div`
    position: fixed;
    z-index: 1000;
    inset: 0;
    background: rgba(0, 0, 0, 0.5);
    display: flex;
    align-items: center;
    justify-content: center;
`;

export const FullPageModal = styled.div`
    background: #a9bcc3;
    border-radius: 24px;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.26);
    padding: 16px;
    display: flex;
    flex-direction: column;
    min-width: 360px;
    min-height: 360px;
    max-width: 96vw;
    max-height: 96vh;
    justify-content: center;
    align-items: center;
    position: relative;
`;

export const ModalGrid = styled.div`
    display: grid;
    grid-template-columns: 1fr 1fr;
    grid-template-rows: 1fr 1fr;
    gap: 15px;
    width: 420px;
    height: 420px;
`;

export const ModalButton = styled.button`
    font-size: 1.25rem;
    color: black;
    font-style: normal;
    font-weight: 500;
    padding: 20px;
    background: #e4e9ee;
    border: none;
    border-radius: 16px;
    cursor: pointer;
    transition: background 0.2s;
    &:hover {
        background: #d1d9e6;
    }
    &:active {
        background: #b8c5d1;
    }
`;

export const SideBar = styled.div`
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 15px 8px;
    background: #a9bcc3;
    box-sizing: border-box;
    box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.25);
    backdrop-filter: blur(10px);
    border-radius: 15px;
    width: 100%;
    height: 100%;
    min-width: 60px;
    gap: 10px;
`;

export const SideBarButtonsWrapper = styled.div`
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 12px;
    flex: 1 1 auto;
    width: 100%;
`;

export const SideBarButton = styled.button`
    width: 48px;
    height: 48px;
    background: #d1d9e6;
    border: none;
    border-radius: 10px;
    padding: 8px 10px;
    margin-bottom: 6px;
    cursor: pointer;
    font-size: 16px;
    transition: background 0.3s;
    box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.2);
    &:hover {
        background: white;
    }
    &:active {
        background: #b8c5d1;
    }
`;

export const BottomButtonWrapper = styled.div`
    width: 100%;
    display: flex;
    justify-content: center;
    align-items: flex-end;
    margin-bottom: 2px;
`;
export const BottomButton = styled.button`
    background: #f5f6fa;
    border: 2px solid #e3e6ec;
    border-radius: 50%;
    width: 54px;
    height: 54px;
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.13);
    transition: background 0.3s, box-shadow 0.2s;

    &:hover {
        background: #849299;
        box-shadow: 0px 6px 18px rgba(0, 0, 0, 0.28);
        border: 2px solid #849299;
    }

    &:hover svg {
        stroke: white;
    }

    &:active {
        background: #e8ebf0;
        box-shadow: 0px 2px 8px rgba(0, 0, 0, 0.31);
    }
`;
