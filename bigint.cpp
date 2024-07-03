#include <iostream>
#include <string.h>
#include <cmath>
#include <cstdint>

char g_Digit[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuwvxyz+/";

template <size_t Base = 16, typename Digit = uint16_t>
class BigInt {
  private:
    Digit *m_Data;
    size_t m_Magnitude;
  public:
    size_t Magnitude() const {
      return m_Magnitude;
    }
    Digit At(size_t n) const {
      if (n >= Magnitude()) return 0;
      return m_Data[n];
    }
    Digit& At(size_t n) {
      return m_Data[n];
    }

    BigInt(Digit *data, size_t magnitude) {
      m_Data = (Digit*) malloc(sizeof(Digit) * magnitude);
      if (data == nullptr) memset(m_Data, 0, magnitude);
      else m_Data = data;
      m_Magnitude = magnitude;
    };

    BigInt(uint64_t n) {
      if (n == 0) m_Magnitude = 1;
      else m_Magnitude = std::ceil(std::logl(n) / std::logl(Base)) + 1;
      m_Data = (Digit*) malloc(sizeof(Digit) * m_Magnitude);
      memset(m_Data, 0, m_Magnitude);
//      std::cerr << n << " - " << m_Data << "\n";
      for (int i = 0; i < m_Magnitude; i++) {
        m_Data[i] = n % Base;
        n /= Base;
      }
    };

    operator uint64_t() const {
      uint64_t b = 1;
      uint64_t sum = 0;
      for (int i = 0; i < m_Magnitude; i++) {
        sum += b * At(i);
        b *= Base;
      }
      return sum;
    }

    operator std::string() const {
      size_t sig_digs = 1;
      for (size_t i = m_Magnitude-1; i >= 0; i--) {
        if (At(i) != 0) {
          sig_digs = i+1;
          break;
        }
      }
      std::string s(sig_digs, ' ');
      for (int i = 0; i < sig_digs; i++) {
        s[sig_digs-i-1] = g_Digit[At(i)];
      }
      return s;
    }

    template <size_t OtherBase>
    operator BigInt<OtherBase, Digit>() {
      size_t new_magnitude = std::ceil(Magnitude() * std::logl(Base) / std::logl(OtherBase));
      BigInt<OtherBase, Digit> sum(0);
      BigInt<OtherBase, Digit> b(1);
      const BigInt<OtherBase, Digit> cbase(Base);
      for (int i = 0; i < Magnitude(); i++) {
        sum += b * At(i);
        b = b * cbase;
//        std::cerr << std::uint64_t(b) << "\n";
      }
      return sum;
    }

    ~BigInt() {
//      std::cout << "Freeing n = " << uint64_t(*this) << "\n";
//      free(m_Data);
    }


    BigInt<Base, Digit> operator<<(size_t n) {
      size_t new_magnitude = Magnitude() + n;
      Digit *new_data = (Digit*) malloc(sizeof(Digit) * new_magnitude);
      for (int i = 0; i < n; i++)
        new_data[i] = 0;
      for (int i = n; i < new_magnitude; i++)
        new_data[i] = At(i - n);
      BigInt<Base, Digit> b(new_data, new_magnitude);
      return b;
    };

    BigInt<Base, Digit> operator+(const BigInt<Base, Digit> &other) const {
      size_t new_magnitude = other.Magnitude() == Magnitude() ? Magnitude() + 1 : std::max(Magnitude(), other.Magnitude());
      Digit *new_data = (Digit*) malloc(sizeof(Digit) * new_magnitude);
      Digit carry = 0;
      for (int i = 0; i < new_magnitude; i++) {
        new_data[i] = (At(i) + other.At(i) + carry) % Base;
        carry = At(i) + other.At(i) >= Base;
      }
      BigInt<Base, Digit> b(new_data, new_magnitude);
      return b;
    }
    void operator+=(const BigInt<Base, Digit> &other) {
      auto m = *this + other;
      m_Magnitude = m.Magnitude();
      for (int i = 0; i < m.Magnitude(); i++) {
        m_Data[i] = m.At(i);
      }
    }
    BigInt<Base, Digit> operator-(const BigInt<Base, Digit> &other) const {
      size_t new_magnitude = std::max(Magnitude(), other.Magnitude()) + 1;
      Digit *new_data = (Digit*) malloc(sizeof(Digit) * new_magnitude);
      Digit carry = 0;
      for (int i = 0; i < new_magnitude; i++) {
        const Digit a = At(i);
        const Digit b = other.At(i);
        Digit c;
        if (a > b) {
          c = a - b - carry;
          carry = 0;
        }
        if (b > a) {
          c = (a + Base) - b - carry;
          carry = 1;
        }
        if (a == b) {
          c = carry ? Base - 1 : 0;
        }
        new_data[i] = c;
        std::cerr << a << "-" << b << " (c'=" << carry << ") => " << c << std::endl;
      }
      BigInt<Base, Digit> b(new_data, new_magnitude);
      return b;
    }

    BigInt<Base, Digit> operator*(const Digit c) const {
      size_t new_magnitude = Magnitude() + 1;
      Digit *new_data = (Digit*) malloc(sizeof(Digit) * new_magnitude);
      Digit carry = 0;
      for (int i = 0; i < new_magnitude; i++) {
        new_data[i] = (At(i) * c + carry) % Base;
        carry = (At(i) * c + carry) / Base;
      }
      BigInt<Base, Digit> b(new_data, new_magnitude);
      return b;
    }

    BigInt<Base, Digit> operator*(const BigInt<Base, Digit> &other) const {
      size_t new_magnitude = Magnitude() + other.Magnitude();
      BigInt<Base, Digit> b(0);

      for (size_t i = 0; i < Magnitude(); i++) {
//        std::cerr << uint64_t(other) << " * " << At(i) << " = " << other * At(i) << "\n";
        b += (other * At(i)) << i;
      }

      return b;
    }

};

int main() {
  int n; std::cin >> n;
  BigInt<10> a(0), b(1);
  for (int i = 0; i < n; i++) {
    BigInt c = b;
    b = b + a;
    a = c;
  }
  std::cout << std::string(BigInt<10>(b)) << std::endl;
}
