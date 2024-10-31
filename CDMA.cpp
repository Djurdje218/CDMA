#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <thread>
#include <mutex>

std::mutex mtx; 


std::vector<std::vector<int>> generateWalshCodes()
{
    return {{1, 1, 1, 1},
            {1, -1, 1, -1},
            {1, 1, -1, -1},
            {1, -1, -1, 1}};
}

std::string textToBinary(const std::string& text) 
{
    std::string binaryString;
    for (char c : text) 
      binaryString += std::bitset<8>(c).to_string(); 
    
    return binaryString;
}

std::vector<int> encodeMessage(const std::string& binaryMessage, const std::vector<int>& walshCode) 
{
    std::vector<int> encodedSignal;
    for (char bit : binaryMessage) {
        int bitValue = (bit == '1') ? 1 : -1;
        for (int walshBit : walshCode) {
            encodedSignal.push_back(bitValue * walshBit);
        }
    }
    return encodedSignal;
}

std::vector<int> combineSignals(const std::vector<std::vector<int>>& signals) 
{
    std::vector<int> combinedSignal(signals[0].size(), 0);
    for (const auto& signal : signals) 
    {
        for (size_t i = 0; i < signal.size(); ++i) 
        {
            combinedSignal[i] += signal[i];
        }
        
    }
    return combinedSignal;
}


std::string decodeMessage(const std::vector<int>& combinedSignal, const std::vector<int>& walshCode) 
{
    std::string decodedBinary;
    int codeLength = walshCode.size();

    for (size_t i = 0; i < combinedSignal.size(); i += codeLength) 
    {
        int sum = 0;
        for (int j = 0; j < codeLength; ++j) 
        {
            sum += combinedSignal[i + j] * walshCode[j];
        }
        decodedBinary += (sum > 0) ? '1' : '0';
    }

    std::string decodedText;
    for (size_t i = 0; i < decodedBinary.size(); i += 8) 
    {
        decodedText += static_cast<char>(std::bitset<8>(decodedBinary.substr(i, 8)).to_ulong());
    }
    return decodedText;
}

int main() 
{
    setlocale(LC_ALL, "Russian");

    auto walshCodes = generateWalshCodes();
    std::vector<std::string> words = {"GOD", "CAT", "HAM", "SUN"};
    std::vector<std::vector<int>> signals(4); // Each thread will store signal here

    // launch threads every transmitter
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&, i]() 
        {
            std::string binaryMessage = textToBinary(words[i]);
            auto encodedSignal = encodeMessage(binaryMessage, walshCodes[i]);

            std::lock_guard<std::mutex> lock(mtx); // lock
            signals[i] = std::move(encodedSignal); 
        });
    }

    // join threads 
    for (auto& t : threads)
    {
        t.join();
    }

    std::vector<int> combinedSignal = combineSignals(signals);

    for (int i = 0; i < 4; ++i) 
    {
        std::string decodedMessage = decodeMessage(combinedSignal, walshCodes[i]);
        std::cout << "Станция " << char(i + 65) << " вещает  " << decodedMessage << "\n";
    }

    return 0;
}
