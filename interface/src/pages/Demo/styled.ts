import styled from 'styled-components';
import { ChatPageWrapper } from '@components/ChatPageWrapper';

export const Wrapper = styled(ChatPageWrapper)`
    display: grid;
    grid-template-areas:
        'sidebar chat scgViewer'
        'sidebar chat scgViewer';
    grid-template-columns: 70px 1fr 1fr;
    grid-gap: 11px;
    height: 100%;
    width: 100%;
`;

export const SideBarGridArea = styled.div`
    grid-area: sidebar;
`;

export const ChatWrapper = styled.div`
    grid-area: chat;
`;

export const SCgViewerWrapper = styled.div`
    grid-area: scgViewer;
    box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
    border-radius: 15px;
    height: 100%;
    display: flex;
    flex-direction: column;
    justify-content: space-around;
    align-items: center;

    .button {
        background-color: rgba(255, 255, 255, 1);
        color: black;
        border-radius: 10px;
        box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
        border: 1px solid black;
        width: 30%;
        height: 6%;
    }
`;

export const OurWrapper = styled.div`
    position: absolute;
    bottom: 7.9vh;
    left: 50.5%;
    box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
    border-radius: 15px;
    height: 31%;
    width: 48.3%;

    display: flex;
    flex-direction: column;
    justify-content: space-around;
    align-items: center;

    color: black;

    .input {
        width: 90%;
        height: 4vh;
        border-radius: 10px;
        box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
    }
`;

export const PopupWrapper = styled.div`
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: rgba(0, 0, 0, 0.5);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 9999;

    .popup {
        background-color: rgba(255, 255, 255, 1);
        width: 35%;
        height: 80%;
        display: flex;
        flex-direction: column;
        padding: 10px;
        color: black;
        border: 1px solid gray;
        border-radius: 20px;
        position: relative;

        h2 {
            font-weight: bold;
            margin-top: 10px;
        }

        h4 {
            padding-left: 10px;
        }

        .numdiv {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .numdiv p {
            width: 40px;
            height: 40px;
            border-radius: 40px;
            color: white;
            display: flex;
            justify-content: center;
            align-items: center;
            font-size: 20px;
            font-weight: bold;
        }

        .state-1 {
            margin-right: 40px;
            background: #849299;
            border: 1px solid #88afe2;
        }

        .state-2 {
            background: #39484d;
            border: 1px solid #3c547b;
        }

        .state-3 {
            margin-right: 40px;
            background: #39484d;
            border: 1px solid #3c547b;
        }

        .state-4 {
            background: #849299;
            border: 1px solid rgb(203, 203, 203);
        }

        input {
            width: 96%;
            height: 4vh;
            border-radius: 20px;
            box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
            border: 1px solid gray;
            margin-bottom: 7px;
            padding-left: 10px;
            cursor: pointer;
        }

        input::placeholder {
            color: grey;
        }

        .close_button {
            color: black;
            background: none;
            border: none;
            padding: 3px;
            font-size: 30px;
            position: absolute;
            right: 20px;
            top: 16px;
            width: 30px;
            height: 30px;
            cursor: pointer;
            transition: box-shadow 0.3s;
        }

        .form {
            padding-left: 10px;
        }

        .button {
            color: white;
            border-radius: 20px;
            box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
            width: 94%;
            height: 6%;
        }

        .next {
            background-color: #39484d;
            border: 1px solid #849299;
            position: absolute;
            bottom: 70px;
            left: 3%;
            font-size: 18px;
            cursor: pointer;
            box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.13);
            transition: background 0.3s, box-shadow 0.2s, color 0.2s;

            &:hover {
                background: #849299;
                box-shadow: 0px 6px 18px rgba(0, 0, 0, 0.28);
                border: 2px solid #849299;
                color: black;
            }

            &:active {
                background: #e8ebf0;
                box-shadow: 0px 2px 8px rgba(0, 0, 0, 0.31);
            }
        }

        .save {
            background-color: #39484d;
            border: 1px solid #849299;
            position: absolute;
            bottom: 10px;
            left: 3%;
            color: white;
            font-size: 18px;
            cursor: pointer;
            box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.13);
            transition: background 0.3s, box-shadow 0.2s, color 0.2s;

            &:hover {
                background: #849299;
                box-shadow: 0px 6px 18px rgba(0, 0, 0, 0.28);
                border: 2px solid #849299;
                color: black;
            }

            &:active {
                background: #e8ebf0;
                box-shadow: 0px 2px 8px rgba(0, 0, 0, 0.31);
            }
        }

        .chips {
            width: 100%;
            display: flex;
            align-items: left;
            flex-direction: column;
        }

        .chip-div {
            display: flex;
            flex-direction: row;
            max-width: 100%;
            padding-right: 10px;
        }

        .chip {
            background-color: rgba(255, 255, 255, 1);
            color: black;
            border-radius: 20px;
            box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
            border: 1px solid gray;
            width: 100%;
            padding-left: 10px;
            padding-right: 10px;
            margin-bottom: 5px;
            height: 4vh;
            display: flex;
            align-items: center;
            justify-content: space-around;
        }

        .chip-delete {
            background-color: rgba(255, 255, 255, 1);
            color: gray;
            border-radius: 25px;
            box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
            border: 1px solid gray;
            height: 4vh;
            width: 4vh;
            margin-left: 5px;
            display: flex;
            align-items: center;
            justify-content: center;
            cursor: pointer;
        }

        .chip-input {
            width: 95%;
            height: 4vh;
            border-radius: 20px;
            box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
            border: 1px solid gray;
            margin-left: 10px;
            margin-bottom: 20px;

            &::placeholder {
                color: gray;
            }
        }

        .fade-in-out {
            animation: fade-in-out-animation 1s ease-in-out;
        }

        @keyframes fade-in-out-animation {
            0% {
                opacity: 0;
            }
            50% {
                opacity: 1;
            }
            100% {
                opacity: 0;
            }
        }

        .hidden {
            display: none;
        }

        .finish-label {
            border: 2px solid #1dd300;
            border-radius: 20px;
            height: 30px;
            color: #1dd300;
            background-color: white;
        }
    }
`;

export const FormInput = styled.input`
    height: 4vh;
    border-radius: 20px;
    box-shadow: 10px 4px 15px rgb(0 0 0 / 25%);
    border: 1px solid gray;
    margin-bottom: 7px;ch
    padding-left: 10px;
`;
