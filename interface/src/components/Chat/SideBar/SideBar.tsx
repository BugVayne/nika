import React from 'react';
import {
    SideBar,
    SideBarButtonsWrapper,
    SideBarButton,
    BottomButtonWrapper,
    BottomButton,
    FullPageModal,
    FullPageModalBackdrop,
    ModalButton,
    ModalGrid,
} from './styled';
import { useState } from 'react';
import { callSwitchToAudioModeAgent } from '@api/sc/agents/switchToAudioModeAgent';
import { ReactComponent as PlusIcon } from '@assets/icon/plus-icon.svg';
import { ReactComponent as TGIcon } from '@assets/icon/telegram-bot.svg';
import { ReactComponent as SoundOn } from '@assets/icon/voicing-on-icon.svg';
import { ReactComponent as SoundOff } from '@assets/icon/voicing-off-icon.svg';

interface SidebarProps {
    onButtonClick?: (index: number) => void;
    onBottomButtonClick?: () => void;
    buttons?: Array<{ label: string; icon?: React.ReactNode; link: string }>;
}

const buttonsData = [
    { label: 'NIKA Telegram Bot', icon: <TGIcon />, link: 'https://web.telegram.org/k/#@ILEsistem_bot' },
    { label: 'Messege voicing', icon: <SoundOn />, link: '' },
];

export const SideBarPanel: React.FC<SidebarProps> = ({ onButtonClick, onBottomButtonClick, buttons = buttonsData }) => {
    const HandleBottomButtonClick = () => {
        HandleBottomButtonClick();
    };

    const [isTalking, setIsTalking] = useState(false);
    const talking = async () => {
        if (isTalking) {
            setIsTalking(false);
            console.log(isTalking);
            await callSwitchToAudioModeAgent('pause');
        } else {
            setIsTalking(true);
            console.log(isTalking);
            await callSwitchToAudioModeAgent('talking');
        }
    };

    const handleButtonClick = (index: number) => {
        if (index === 0 && buttons[index].link) {
            window.open(buttons[index].link, '_blank');
        } else if (index === 1) {
            talking();
            onButtonClick && onButtonClick(index);
        } else {
            onButtonClick && onButtonClick(index);
        }
    };
    const getVoicingIcon = () => {
        return isTalking ? <SoundOn /> : <SoundOff />;
    };
    const updatedButtons = buttons.map((btn, index) => {
        if (index === 1) {
            return { ...btn, icon: getVoicingIcon() };
        }
        return btn;
    });
    return (
        <SideBar>
            <SideBarButtonsWrapper>
                {updatedButtons.map((btn, idx) => (
                    <SideBarButton key={idx} onClick={() => handleButtonClick(idx)} title={btn.label}>
                        {btn.icon ? btn.icon : btn.label}
                    </SideBarButton>
                ))}
            </SideBarButtonsWrapper>
            <BottomButtonWrapper>
                <BottomButton onClick={onBottomButtonClick} title="Add" aria-label="Add">
                    <PlusIcon />
                </BottomButton>
            </BottomButtonWrapper>
        </SideBar>
    );
};

interface GridModalProps {
    open: boolean;
    onClose: () => void;
    buttonLabels: string[];
    buttonHandlers: Array<() => void>;
}

export function GridModal({ open, onClose, buttonLabels, buttonHandlers }: GridModalProps) {
    if (!open) return null;
    return (
        <FullPageModalBackdrop
            onClick={(e) => {
                if (e.target === e.currentTarget) onClose();
            }}
        >
            <FullPageModal>
                <ModalGrid>
                    {buttonLabels.map((label, i) => (
                        <ModalButton
                            key={label}
                            onClick={() => {
                                buttonHandlers[i]?.();
                                onClose();
                            }}
                        >
                            {label}
                        </ModalButton>
                    ))}
                </ModalGrid>
            </FullPageModal>
        </FullPageModalBackdrop>
    );
}
