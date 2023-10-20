#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

struct BigNumber {
    std::vector<unsigned char> digits;

    bool operator<(const BigNumber& other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() < other.digits.size();
        }
        for (int i = digits.size() - 1; i >= 0; --i) {
            if (digits[i] != other.digits[i]) {
                return digits[i] < other.digits[i];
            }
        }
        return false;
    }

    bool operator>=(const BigNumber& other) const {
        return !(*this < other);
    }

    void setNumberFromHexString(const std::string& hexString) {
        digits.clear();
        for (size_t i = hexString.length(); i >= 2; i -= 2) {
            std::stringstream ss;
            ss << std::hex << hexString.substr(i - 2, 2);
            unsigned int digit;
            ss >> digit;
            digits.push_back(static_cast<unsigned char>(digit));
        }
    }

    std::string getNumberAsHexString() {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            ss << std::setw(2) << static_cast<unsigned>(*it);
        }
        std::string result = ss.str();

        result.erase(0, result.find_first_not_of('0'));
        return result.empty() ? "0" : result;
    }

    BigNumber XOR(const BigNumber& other) const {
        BigNumber result;
        size_t maxLength = std::max(digits.size(), other.digits.size());
        result.digits.resize(maxLength);
        for (size_t i = 0; i < maxLength; ++i) {
            unsigned char a = i < digits.size() ? digits[i] : 0;
            unsigned char b = i < other.digits.size() ? other.digits[i] : 0;
            result.digits[i] = a ^ b;
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber INV() const {
        BigNumber result;
        result.digits.resize(digits.size(), 0);
        for (size_t i = 0; i < digits.size(); ++i) {
            result.digits[i] = ~digits[i];
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber OR(const BigNumber& other) const {
        BigNumber result;
        size_t max_size = std::max(digits.size(), other.digits.size());
        result.digits.resize(max_size);

        for (size_t i = 0; i < max_size; ++i) {
            unsigned char a = i < digits.size() ? digits[i] : 0;
            unsigned char b = i < other.digits.size() ? other.digits[i] : 0;
            result.digits[i] = a | b;
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber AND(const BigNumber& other) const {
        BigNumber result;
        result.digits.resize(std::max(digits.size(), other.digits.size()), 0);
        for (size_t i = 0; i < result.digits.size(); ++i) {
            unsigned char a = i < digits.size() ? digits[i] : 0;
            unsigned char b = i < other.digits.size() ? other.digits[i] : 0;
            result.digits[i] = a & b;
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber shiftR(int n) const {
        BigNumber result;
        if (n >= 8) {
            return result;
        }
        result.digits.resize(digits.size(), 0);
        unsigned char carry = 0;
        for (size_t i = 0; i < digits.size(); ++i) {
            unsigned char temp = digits[i];
            result.digits[i] = (temp >> n) | carry;
            carry = (temp << (8 - n));
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber shiftL(int n) const {
        BigNumber result;
        if (n >= 8) {
            return result;
        }
        result.digits.resize(digits.size(), 0);
        unsigned char carry = 0;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            unsigned char temp = digits[i];
            result.digits[i] = (temp << n) | carry;
            carry = (temp >> (8 - n));
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber ADD(const BigNumber& other) const {
        BigNumber result;
        unsigned char carry = 0;
        size_t maxSize = std::max(digits.size(), other.digits.size());
        result.digits.resize(maxSize, 0);

        for (size_t i = 0; i < maxSize; ++i) {
            unsigned char a = i < digits.size() ? digits[i] : 0;
            unsigned char b = i < other.digits.size() ? other.digits[i] : 0;
            unsigned char sum = a + b + carry;

            result.digits[i] = sum;
            carry = sum < a || sum < b ? 1 : 0;
        }

        if (carry) {
            result.digits.push_back(carry);
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber SUB(const BigNumber& other) const {
        BigNumber result;
        unsigned char borrow = 0;
        result.digits.resize(digits.size(), 0);

        for (size_t i = 0; i < digits.size(); ++i) {
            unsigned char a = digits[i];
            unsigned char b = i < other.digits.size() ? other.digits[i] : 0;
            unsigned char diff = 0;

            if (a < b + borrow) {
                diff = 256 + a - b - borrow;
                borrow = 1;
            }
            else {
                diff = a - b - borrow;
                borrow = 0;
            }
            result.digits[i] = diff;
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }

    BigNumber MOD(const BigNumber& other) const {
        BigNumber result = *this;
        while (result.digits.size() >= other.digits.size()) {
            while (result >= other) {
                for (size_t i = 0; i < other.digits.size(); ++i) {
                    result.digits[result.digits.size() - 1 - i] -= other.digits[other.digits.size() - 1 - i];
                }

                while (!result.digits.empty() && result.digits.back() == 0) {
                    result.digits.pop_back();
                }
            }

            if (result < other) {
                break;
            }
        }

        while (result.digits.size() > 1 && result.digits.back() == 0) {
            result.digits.pop_back();
        }

        return result;
    }
};

int main() {
    BigNumber bigNumber;
    std::string hexString = "2A3B4C5D";
    bigNumber.setNumberFromHexString(hexString);
    std::string convertedHexString = bigNumber.getNumberAsHexString();
    std::cout << "Converted Hex String: " << convertedHexString << std::endl;

    BigNumber numberA, numberB, numberC;
    numberA.setNumberFromHexString("51bf608414ad5726a3c1bec098f77b1b54ffb2787f8d528a74c1d7fde6470ea4");
    numberB.setNumberFromHexString("403db8ad88a3932a0b7e8189aed9eeffb8121dfac05c3512fdb396dd73f6331c");

    numberC = numberA.XOR(numberB);
    std::cout << "XOR Result: " << numberC.getNumberAsHexString() << std::endl;

    numberC = numberA.INV();
    std::cout << "INV Result: " << numberC.getNumberAsHexString() << std::endl;

    numberC = numberA.OR(numberB);
    std::cout << "OR Result: " << numberC.getNumberAsHexString() << std::endl;

    numberC = numberA.AND(numberB);
    std::cout << "AND Result: " << numberC.getNumberAsHexString() << std::endl;

    int shiftAmount = 2;
    numberC = numberA.shiftR(shiftAmount);
    std::cout << "Shift Right Result: " << numberC.getNumberAsHexString() << std::endl;

    numberC = numberA.shiftL(shiftAmount);
    std::cout << "Shift Left Result: " << numberC.getNumberAsHexString() << std::endl;

    numberC = numberA.ADD(numberB);
    std::cout << "ADD Result: " << numberC.getNumberAsHexString() << std::endl;

    numberC = numberA.SUB(numberB);
    std::cout << "SUB Result: " << numberC.getNumberAsHexString() << std::endl;

    numberC = numberA.MOD(numberB);
    std::cout << "MOD Result: " << numberC.getNumberAsHexString() << std::endl;

    return 0;
}