close all; clear all;
%% Simulation Parameters 
SNR = -5:20;           % Sweep Signal to Noise Ratio in dBs
D   = 0.25:0.1:0.75;  % Sweep DC offset values (assuming different antenna have different DC offset values)
N   = 512;            % Number of time domain samples
fs  = 4000;           % Sampling frequency in Hz(assuming 4Khz similar to what testData.txt contains)
f   = 400;            % Signal frequency in Hz(assuming 1Khz similar to what testData.txt contains)
% Different scenarios to simulate theEffect of NOISE & DC OFFSET on using two complex sample to estimate the frequency
testCase = ["NoAvg with DC", "NoAvg without DC Offset" "MovingAvg with DC" "MovingAvg without DC Offset"]; 
%% For Spectral Analysis
Nfft= N;               % FFT Size
dt  = 1/fs;
t   = (0:Nfft-1)*dt ;  % time span in sec
df  = 1/(Nfft*dt);     % bin size in Hz
bins= (0:Nfft-1)*df;   % freq span
%% 
n   = 0:N-1;
windowSize = 2;  % For Noise smoothning Adjust the window size as needed 

for plt=1:length(testCase)
    figure(1)
    subplot(length(testCase),1,plt)
    for snrIdx= 1: length(SNR)
        for dc_idx= 1: length(D)
            y =awgn(exp(1j*2*pi*f*n/fs), SNR(snrIdx), 'measured') + D(dc_idx);
            %scatterplot(y)
            %stem(bins,(abs(fft(y,Nfft))/Nfft))
            DC_off = abs(mean(y));  % DC Offset Estimation
            switch testCase(plt)
                case 'NoAvg with DC'
                    phaseDiff    = angle(y(2:end)) - angle(y(1:end-1));
                case 'NoAvg without DC Offset'
                    phaseDiff    = angle(y(2:end)-DC_off) - angle(y(1:end-1)-DC_off);  % Remove the estimated DC offset from 'y'
                case 'MovingAvg with DC'
                    ySmooth      = movmean(y, windowSize);   % Noise smoothning using Moving Average Filter    
                    phaseDiff    = angle(ySmooth(2:end)) - angle(ySmooth(1:end-1));
                case 'MovingAvg without DC Offset' 
                    ySmooth      = movmean(y, windowSize);  % Noise smoothning using Moving Average Filter
                    phaseDiff    = angle(ySmooth(2:end)-DC_off) - angle(ySmooth(1:end-1)-DC_off);  % Remove the estimated DC offset from smoothen signal
                otherwise
                    disp('other value')
            end

            negative_idx = phaseDiff < 0;
            phaseDiff(negative_idx) = phaseDiff(negative_idx) +2*pi;
            estimatedFrequency(snrIdx,dc_idx) = mean((phaseDiff .* fs) /(2* pi));
            %X = sprintf('DC_off: %d Frequency Estimate: %d Hz\n',DC_off, estimatedFrequency);
            %disp(X)
        end
    end

    % plot estimated Frequency for different SNR & antenna (here DC offset)
    for dc_idx= 1: length(D)
        plot(SNR, estimatedFrequency(:,dc_idx))
        legendInfo{dc_idx} = ['DC Offset ' num2str(D(dc_idx))];
        hold on
    end
    % plot original Frequency
    plot(SNR, ones(length(SNR),1)*f)

    legendInfo{length(D)+1} = ['Original Freq ' num2str(f)];
    hold off;
    % Add the legend with all entries
    if (plt==1)
        legend(legendInfo)
    end
    title(sprintf("Case: %s",testCase(plt))) 
        ylim([300 2100])
    xlabel('SNR'); ylabel('FreqEst(in Hz)')
end






