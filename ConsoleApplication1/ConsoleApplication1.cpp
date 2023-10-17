#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

struct BigNumber {
    std::vector<uint64_t> digits;

    void setNumberFromHexString(std::string hexString) {
        digits.clear();
        for (size_t i = hexString.length(); i >= 16; i -= 16) {
            std::stringstream ss;
            ss << std::hex << hexString.substr(i - 16, 16);
            uint64_t digit;
            ss >> digit;
            digits.push_back(digit);
        }
        if (hexString.length() % 16 != 0) {
            std::stringstream ss;
            ss << std::hex << hexString.substr(0, hexString.length() % 16);
            uint64_t digit;
            ss >> digit;
            digits.push_back(digit);
        }
    }

    std::string getNumberAsHexString() {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            ss << std::setw(16) << *it;
        }
        std::string result = ss.str();

        result.erase(0, result.find_first_not_of('0'));
        return result.empty() ? "0" : result;
    }

    BigNumber XOR(const BigNumber& other) const {
        BigNumber result;
        result.digits.resize(std::max(digits.size(), other.digits.size()), 0);
        for (size_t i = 0; i < result.digits.size(); ++i) {
            uint64_t a = i < digits.size() ? digits[i] : 0;
            uint64_t b = i < other.digits.size() ? other.digits[i] : 0;
            result.digits[i] = a ^ b;
        }
        return result;
    }

    BigNumber INV() const {
        BigNumber result;
        result.digits.resize(digits.size(), 0);
        for (size_t i = 0; i < digits.size(); ++i) {
            result.digits[i] = ~digits[i];
        }
        return result;
    }

    BigNumber OR(const BigNumber& other) const {
        BigNumber result;
        result.digits.resize(std::max(digits.size(), other.digits.size()), 0);
        for (size_t i = 0; i < result.digits.size(); ++i) {
            uint64_t a = i < digits.size() ? digits[i] : 0;
            uint64_t b = i < other.digits.size() ? other.digits[i] : 0;
            result.digits[i] = a | b;
        }
        return result;
    }

    BigNumber AND(const BigNumber& other) const {
        BigNumber result;
        result.digits.resize(std::max(digits.size(), other.digits.size()), 0);
        for (size_t i = 0; i < result.digits.size(); ++i) {
            uint64_t a = i < digits.size() ? digits[i] : 0;
            uint64_t b = i < other.digits.size() ? other.digits[i] : 0;
            result.digits[i] = a & b;
        }
        return result;
    }

    BigNumber shiftR(int n) const {
        BigNumber result;
        if (n >= 64) {
            return result;
        }
        result.digits.resize(digits.size(), 0);
        uint64_t carry = 0;
        for (size_t i = 0; i < digits.size(); ++i) {
            uint64_t temp = digits[i];
            result.digits[i] = (temp >> n) | carry;
            carry = (temp << (64 - n));
        }
        return result;
    }

    BigNumber shiftL(int n) const {
        BigNumber result;
        if (n >= 64) {
            return result;
        }
        result.digits.resize(digits.size(), 0);
        uint64_t carry = 0;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            uint64_t temp = digits[i];
            result.digits[i] = (temp << n) | carry;
            carry = (temp >> (64 - n));
        }
        return result;
    }

    BigNumber ADD(const BigNumber& other) const {
        BigNumber result;
        uint64_t carry = 0;
        size_t maxSize = std::max(digits.size(), other.digits.size());
        result.digits.resize(maxSize, 0);

        for (size_t i = 0; i < maxSize; ++i) {
            uint64_t a = i < digits.size() ? digits[i] : 0;
            uint64_t b = i < other.digits.size() ? other.digits[i] : 0;
            uint64_t sum = a + b + carry;

            result.digits[i] = sum;
            carry = sum < a || sum < b ? 1 : 0;
        }

        if (carry) {
            result.digits.push_back(carry);
        }
        return result;
    }

    BigNumber SUB(const BigNumber& other) const {
        BigNumber result;
        uint64_t borrow = 0;
        result.digits.resize(digits.size(), 0);

        for (size_t i = 0; i < digits.size(); ++i) {
            uint64_t a = digits[i];
            uint64_t b = i < other.digits.size() ? other.digits[i] : 0;
            uint64_t diff = 0;

            if (a < b + borrow) {
                diff = (1ULL << 64) + a - b - borrow;
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
};

int main() {
    BigNumber bigNumber;
    std::string hexString = "2A3B4C5D";
    bigNumber.setNumberFromHexString(hexString);
    std::string convertedHexString = bigNumber.getNumberAsHexString();
    std::cout << "Converted Hex String: " << convertedHexString << std::endl;

    BigNumber numberA, numberB, numberC;
    numberA.setNumberFromHexString("7d7deab2affa38154326e96d350deee1");
    numberB.setNumberFromHexString("97f92a75b3faf8939e8e98b96476fd22");

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

    return 0;
}