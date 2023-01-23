#include "ustring.hpp"

#include <array>

/*
    UString iterator
*/

UString::iterator::iterator(const UString& ustr)
    : m_ustring(ustr.m_ustring), m_length(ustr.length()) {}

UString::iterator::iterator(const iterator& it)
    : m_ustring(it.m_ustring), m_idx(it.m_idx), m_length(it.m_length) {}

UString::iterator& UString::iterator::operator=(const UString::iterator& it) {
    m_ustring = it.m_ustring;
    m_idx = it.m_idx;
    m_length = it.m_length;
    return *this;
}

UString::iterator::reference UString::iterator::operator*() const {
    if (m_idx >= m_length) {
        throw std::out_of_range("index value is greater than the length of the string");
    }
    auto pos = get_codepoint_pos(m_idx, m_ustring);
    return m_ustring.substr(pos, get_codepoint_len(pos, m_ustring));
}

UString::iterator::pointer UString::iterator::operator->() const {
    return nullptr;
}

UString::iterator& UString::iterator::operator++() {
    ++m_idx;
    return *this;
}

UString::iterator UString::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

UString::iterator& UString::iterator::operator--() {
    --m_idx;
    return *this;
}

UString::iterator UString::iterator::operator--(int) {
    iterator tmp = *this;
    --(*this);
    return tmp;
}

UString::iterator::difference_type UString::iterator::operator-(const iterator& it) const {
    return m_idx - it.m_idx;
}

bool UString::iterator::operator==(const iterator& it) const {
    return m_idx == it.m_idx && m_length == it.m_length;
}

bool UString::iterator::operator!=(const iterator& it) const {
    return !(*this == it);
}

bool UString::iterator::operator<(const iterator& it) const {
    return m_idx < it.m_idx;
}

bool UString::iterator::operator>(const iterator& it) const {
    return m_idx > it.m_idx;
}

bool UString::iterator::operator<=(const iterator& it) const {
    return m_idx <= it.m_idx;
}

bool UString::iterator::operator>=(const iterator& it) const {
    return m_idx >= it.m_idx;
}

UString::iterator UString::iterator::operator+(size_t n) const {
    iterator res = *this;
    res.m_idx += n;
    return res;
}

UString::iterator UString::iterator::operator-(size_t n) const {
    iterator res = *this;
    res.m_idx -= n;
    return res;
}


/*
    UString
*/

UString::UString(const char* cstr): UString(std::string(cstr)) {}

UString::UString(const std::string& str): m_ustring(str) {
    if (!is_well()) {
        throw std::invalid_argument("invalid UTF-8 string");
    }
    m_length = calc_length();
}

UString::UString(const UString& other)
    : m_ustring(other.m_ustring), m_length(other.m_length) {}

UString::UString(UString&& other) noexcept
    : m_ustring(std::move(other.m_ustring)), m_length(std::move(other.m_length)) {}

UString& UString::operator=(const char* str) {
    return (*this = std::string(str));
}

UString& UString::operator=(const std::string& str) {
    auto old = m_ustring;
    m_ustring = str;
    if (!is_well()) {
        m_ustring = old;
        throw std::invalid_argument("invalid UTF-8 string");
    }
    m_length = calc_length();
    return *this;
}

UString& UString::operator=(const UString& other) {
    m_ustring = other.m_ustring;
    m_length = other.m_length;
    return *this;
}

UString& UString::operator=(UString&& other) noexcept {
    m_ustring = std::move(other.m_ustring);
    m_length = std::move(other.m_length);
    return *this;
}

UString& UString::operator+=(const std::string& str) {
    auto old = m_ustring;
    m_ustring += str;
    if (!is_well()) {
        m_ustring = old;
        throw std::invalid_argument("invalid UTF-8 string");
    }
    m_length = calc_length();
    return *this;
}

UString& UString::operator+=(const char* cstr) {
    return (*this += std::string(cstr));
}

UString& UString::operator+=(const UString& other) {
    m_ustring += other.m_ustring;
    m_length += other.m_length;
    return *this;
}

void UString::clear() noexcept {
    m_ustring.clear();
    m_length = 0;
}

bool UString::empty() const noexcept {
    return m_length == 0;
}

bool UString::is_well() const {
    /*
        According to the table from:
        https://lemire.me/blog/2018/05/09/how-quickly-can-you-check-that-a-string-is-valid-unicode-utf-8/
    */

    auto bytes = std::basic_string<unsigned char>(m_ustring.cbegin(), m_ustring.cend());
    auto it = bytes.cbegin();
    auto end = bytes.cend(); 
    while (it != end) {
        if ((0xF8 & *it) == 0xF1 && *it <= 0xF4) {
            if (it + 1 == end || it + 2 == end || it + 3 == end) {
                return false;
            }

            if ((0xC0 & *(it + 1)) != 0x80 || (0xC0 & *(it + 2)) != 0x80 || (0xC0 & *(it + 3)) != 0x80) {
                return false;
            }

            if (*it == 0xF0) {
                if (*(it + 1) < 0x90 || *(it + 1) > 0xBF) {
                    return false;
                }
            } else if (*it == 0xF4) {
                if (*(it + 1) < 0x80 || *(it + 1) > 0x8F) {
                    return false;
                }
            }

            it += 4;
        } else if ((0xF0 & *it) == 0xE0) {
            if (it + 1 == end || it + 2 == end) {
                return false;
            }

            if ((0xC0 & *(it + 1)) != 0x80 || (0xC0 & *(it + 2)) != 0x80) {
                return false;
            }

            if (*it == 0xE0) {
                if (*(it + 1) < 0xA0 || *(it + 1) > 0xBF) {
                    return false;
                }
            } else if (*it == 0xED) {
                if (*(it + 1) > 0x9F) {
                    return false;
                }
            }

            it += 3;
        } else if ((0xE0 & *it) == 0xC0) {
            if (it + 1 == end) {
                return false;
            }

            if ((0xC0 & *(it + 1)) != 0x80) {
                return false;
            }

            it += 2;
        } else if ((0x80 & *it) == 0x00) {
            it += 1;
        } else {
            return false;
        }
    }

    return true;
}

size_t UString::size() const noexcept {
    return m_ustring.size();
}

size_t UString::length() const noexcept {
    return m_length;
}

UString::uchar UString::at(size_t index) const {
    if (index >= m_length) {
        throw std::out_of_range("index value is greater than the length of the string");
    }
    auto pos = get_codepoint_pos(index);
    return m_ustring.substr(pos, get_codepoint_len(pos));
}

UString::uchar UString::operator[](size_t index) const {
    auto pos = get_codepoint_pos(index);
    return m_ustring.substr(pos, get_codepoint_len(pos));
}

void UString::push_back(unsigned int ch) {
    push_back(codepoint_to_string(ch));
}

void UString::push_back(uchar ch) {
    UString other = UString(ch);
    *this += other;
}

void UString::pop_back() {
    if (m_length == 0) {
        throw std::length_error("cannot remove the last element from an empty string");
    }

    auto pos = get_codepoint_pos(m_length - 1);
    m_ustring.erase(pos);
    --m_length;
}

UString::iterator UString::begin() const noexcept {
    return iterator(*this);
}

UString::iterator UString::cbegin() const noexcept {
    return begin();
}

UString::iterator UString::end() const noexcept {
    return begin() + m_length;
}

UString::iterator UString::cend() const noexcept {
    return end();
}

UString::reverse_iterator UString::rbegin() const noexcept {
    return reverse_iterator(end());
}

UString::reverse_iterator UString::crbegin() const noexcept {
    return rbegin();
}

UString::reverse_iterator UString::rend() const noexcept {
    return rbegin() + m_length;
}

UString::reverse_iterator UString::crend() const noexcept {
    return rend();
}

UString operator+(const UString& lhs, const UString& rhs) {
    return lhs + rhs.m_ustring;
}

UString operator+(const UString& lhs, const std::string& rhs) {
    return UString(lhs.m_ustring + rhs);
}

UString operator+(const std::string& lhs, const UString& rhs) {
    return UString(lhs + rhs.m_ustring);
}

UString operator+(const UString& lhs, const char* rhs) {
    return lhs + std::string(rhs);
}

UString operator+(const char* lhs, const UString& rhs) {
    return std::string(lhs) + rhs;
}

bool operator==(const UString& lhs, const UString& rhs) noexcept {
    return lhs.m_ustring == rhs.m_ustring;
}

bool operator!=(const UString& lhs, const UString& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator<=(const UString& lhs, const UString& rhs) noexcept {
    return lhs.m_ustring <= rhs.m_ustring;
}

bool operator>=(const UString& lhs, const UString& rhs) noexcept {
    return lhs.m_ustring >= rhs.m_ustring;
}

bool operator<(const UString& lhs, const UString& rhs) noexcept {
    return lhs.m_ustring < rhs.m_ustring;
}

bool operator>(const UString& lhs, const UString& rhs) noexcept {
    return lhs.m_ustring > rhs.m_ustring;
}

std::ostream& operator<<(std::ostream& os, const UString& ustr) {
    os << ustr.m_ustring;
    return os;
}

std::istream& operator>>(std::istream& is, UString& ustr) {
    std::string tmp;
    char symb;
    do {
        symb = is.get();
        if (symb == '\n' || symb == ' ') {
            break;
        }
        tmp.push_back(symb);
    } while (true);
    ustr = tmp;

    return is;
}

std::string UString::codepoint_to_string(unsigned int code) {
    if (code > 0x10FFFF) {
        throw std::invalid_argument("invalid UTF-8 code");
    }

    std::array<char, 5> bytes = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    if (code <= 0x7F) { 
        bytes[0] = code;
    } else if (code <= 0x7FF) {
        bytes[0] = (code >> 6) + 0b11000000;
        bytes[1] = (code & 0b111111) + 0b10000000;
    } // else if (0xd800 <= code && code <= 0xdfff) {}
    else if (code <= 0xFFFF) {
        bytes[0] = (code >> 12) + 0b11100000;
        bytes[1] = ((code >> 6) & 0b111111) + 0b10000000;
        bytes[2] = (code & 0b111111) + 0b10000000;
    } else if (code <= 0x10FFFF) {
        bytes[0] = (code >> 18) + 0b11111000;
        bytes[1] = ((code >> 12) & 0b111111) + 0b10000000;
        bytes[2] = ((code >> 6) & 0b111111) + 0b10000000;
        bytes[3] = (code & 0b111111) + 0b10000000;
    }
    return bytes.data();
}

size_t UString::get_codepoint_pos(size_t index) const {
    return get_codepoint_pos(index, m_ustring);
}

size_t UString::get_codepoint_len(size_t pos) const {
    return get_codepoint_len(pos, m_ustring);
}

size_t UString::calc_length() const {
    size_t pos = 0;
    size_t len = 0;
    while (pos < m_ustring.size()) {
        pos += get_codepoint_len(pos);
        ++len;
    }
    return len;
}
