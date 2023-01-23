#pragma once

#include <string>
#include <iostream>

class UString {
    using ustring_t = std::string;

public:
    using uchar = std::string;

private:
    struct iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = uchar;
        using pointer           = void*;
        using reference         = uchar;

    public:
        iterator(const UString& ustr);
        iterator(const iterator& it);

        iterator& operator=(const iterator& it);

        reference operator*() const;
        pointer operator->() const;

        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);

        difference_type operator-(const iterator& it) const;

        bool operator==(const iterator& it) const;
        bool operator!=(const iterator& it) const;
        bool operator<(const iterator& it) const;
        bool operator>(const iterator& it) const;
        bool operator<=(const iterator& it) const;
        bool operator>=(const iterator& it) const;

        iterator operator+(size_t n) const;
        iterator operator-(size_t n) const;

    private:
        ustring_t m_ustring;
        size_t m_idx = 0;
        size_t m_length = 0;
    };

public:
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;

public:
    UString() = default;

    UString(const char* cstr);
    UString(const std::string& str);
    UString(const UString& other);
    UString(UString&& other) noexcept;

    UString& operator=(const char* str);
    UString& operator=(const std::string& str);
    UString& operator=(const UString& other);
    UString& operator=(UString&& other) noexcept;

    UString& operator+=(const std::string& str);
    UString& operator+=(const char* cstr);
    UString& operator+=(const UString& other);

    void clear() noexcept;
    bool empty() const noexcept;

    bool is_well() const;

    size_t size() const noexcept;
    size_t length() const noexcept;

    uchar at(size_t index) const;
    uchar operator[](size_t index) const;

    void push_back(unsigned int ch);
    void push_back(uchar ch);
    void pop_back();

    iterator begin() const noexcept;
    iterator cbegin() const noexcept;

    iterator end() const noexcept;
    iterator cend() const noexcept;

    reverse_iterator rbegin() const noexcept;
    reverse_iterator crbegin() const noexcept;

    reverse_iterator rend() const noexcept;
    reverse_iterator crend() const noexcept;

    friend UString operator+(const UString& lhs, const UString& rhs);
    friend UString operator+(const UString& lhs, const std::string& rhs);
    friend UString operator+(const std::string& lhs, const UString& rhs);
    friend UString operator+(const UString& lhs, const char* rhs);
    friend UString operator+(const char* lhs, const UString& rhs);

    friend bool operator==(const UString& lhs, const UString& rhs) noexcept;
    friend bool operator!=(const UString& lhs, const UString& rhs) noexcept;
    friend bool operator<=(const UString& lhs, const UString& rhs) noexcept;
    friend bool operator>=(const UString& lhs, const UString& rhs) noexcept;
    friend bool operator<(const UString& lhs, const UString& rhs) noexcept;
    friend bool operator>(const UString& lhs, const UString& rhs) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const UString& ustr);
    friend std::istream& operator>>(std::istream& is, UString& ustr);

private:
    std::string codepoint_to_string(unsigned int code);

    size_t get_codepoint_pos(size_t index) const;
    size_t get_codepoint_len(size_t pos) const;

    size_t calc_length() const;

private:
    static size_t get_codepoint_pos(size_t index, ustring_t ustring) {
        size_t pos = 0;
        size_t usize = ustring.size();
        for (size_t i = 0; pos < usize && i < index; ++i) {
            pos += get_codepoint_len(pos, ustring);
        }
        return pos;
    }

    static size_t get_codepoint_len(size_t pos, ustring_t ustring) {
        unsigned char byte = ustring[pos];
        if ((0xF8 & byte) == 0xF0) {
            return 4;
        }
        if ((0xF0 & byte) == 0xE0) {
            return 3;
        }
        if ((0xE0 & byte) == 0xC0) {
            return 2;
        } 
        return 1;
    }

private:
    ustring_t m_ustring;
    size_t m_length = 0;
};
