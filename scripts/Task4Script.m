clear all; close all;

% Load data
data = load('testData.txt');

% Sampling frequency
fs = 4000;

% Number of antennas
NumAnt = 8;

% Initialize variables
N  = 512;%size(data, 1);  % Number of samples to process in one shot
dt = 1 / fs;               % sampling time in seconds
t  = (0:N-1) * dt;         % sample acquisition duration in seconds
df = 1 / (N * dt);         % bin size in Hz
f  = -fs/2:df:fs/2-df;     % mapping frequency on x-axis

% Process each antenna's data
for antid = 1:NumAnt    
    % Extract received signals
    y = complex(data(:, 2*antid-1), data(:, 2*antid));  
    y = y(1:N);
    
    %% Task4 Question 3 : To find frequency of recorded signal
    % Fourier Transform and shift the zero frequency to the center
    yK = fftshift(fft(y, N) / N);
	stem(f,(abs(yK)))

    % Find the index and magnitude of the peak frequency
    [peakMagnitude, peakIndex] = max(abs(yK));
    peakFrequency = f(peakIndex);

    % Find the magnitude of the DC frequency component (frequency at 0 Hz)
    % dcMagnitude represents original DC Offset in signal
    dcMagnitude = abs(yK(N/2 + 1));
   
    % DC Offset Estimation using averaging
    dcOffsetEst = abs(mean(y));  

    %% Task4 Question 2 : use two complex sample to estimate the frequency
    % Compute phase differences of two complex sample to estimate the frequency
    phaseDiff   = angle(y(2:end)) - angle(y(1:end-1));    
    negativeIdx = phaseDiff < 0;
    phaseDiff(negativeIdx) = phaseDiff(negativeIdx) + 2*pi;
    % Estimate signal frequency    
    freqEst     = mean(phaseDiff .* fs) / (2 * pi);

    % Remove the DC offset effect & Apply a moving average filter to
    % phaseDiff to reduce noise to get accurate frequency estimation
    windowSize      = 4; % Adjust the window size as needed
    smoothSignal    = movmean(y-dcOffsetEst, windowSize);
    smoothPhaseDiff = angle(smoothSignal(2:end)) - angle(smoothSignal(1:end-1));
    negativeIdx     = smoothPhaseDiff < 0;
    smoothPhaseDiff(negativeIdx) = smoothPhaseDiff(negativeIdx) + 2*pi;
    newFreqEst      = mean(smoothPhaseDiff.* fs) / (2 * pi);
    
    % Display results
    fprintf('Ant[%d] - Original DC Offset: %.4f, Estimated DC Offset: %.4f : Signal Frequency: %.2f Hz, FreqEstimation: %.2f Hz , After Noise smoothning FreqEstimation: %.2f Hz\n', ...
             antid, dcMagnitude, dcOffsetEst, peakFrequency, freqEst, newFreqEst);
end
