#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <cmath>

using namespace std;

bool isPrime(int64_t n)
{
  for (int64_t i = 2; i < sqrt(n); ++i)
  {
    if (n % i == 0)
    {
      return false;
    }
  }
  return true;
}

int64_t gcd(int64_t a, int64_t b)
{
  if (a == b)
    return a;
  if (a < b)
    swap(a, b);

  return gcd(a - b, b);
}

void solveQuadratic(int64_t a, int64_t b, int64_t c, int64_t &root1, int64_t &root2)
{
  int64_t discriminant = b * b - 4 * a * c;

  if (discriminant < 0)
  {
    root1 = root2 = 0;
    return;
  }

  root1 = (-b + sqrt(discriminant)) / (2 * a);
  root2 = (-b - sqrt(discriminant)) / (2 * a);
}

// Did not end up using, but kept for reference & experimenting with different algorithms

// Algorithm from https://www.geeksforgeeks.org/eulers-totient-function-for-all-numbers-smaller-than-or-equal-to-n/#
// and https://cp-algorithms.com/algebra/phi-function.html
// vector<int64_t> phi(int64_t n)
// {
//   // Initialize array of totient values for 1 to n
//   vector<int64_t> phi(n + 1);
//   for (int64_t i = 0; i <= n; i++)
//     phi[i] = i;

//   // Compute phi for all numbers from 2 to n
//   for (int64_t i = 2; i <= n; i++)
//   {
//     if (phi[i] == i)
//     {
//       for (int64_t j = i; j <= n; j += i)
//         phi[j] -= phi[j] / i;
//     }
//   }

//   return phi;
// }

// int64_t phi(int64_t n)
// {
//   int64_t result = n;

//   for (int64_t i = 2; i * i <= n; i++)
//   {
//     // Check if i is a prime factor of n
//     if (n % i == 0)
//     {
//       while (n % i == 0)
//         n /= i;
//       result -= result / i;
//     }
//   }

//   if (n > 1)
//     result -= result / n;

//   return result;
// }

int64_t totient_naive(int64_t n)
{
  int64_t result = 1;
  for (int64_t i = 2; i < n; i++)
  {
    if (gcd(i, n) == 1)
      result++;
  }

  return result;
}

// Algorithm from https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
int64_t gcdExtended(int64_t e, int64_t phi)
{
  int64_t d_old = 0, r_old = phi;
  int64_t d_new = 1, r_new = e;

  while (r_new > 0)
  {
    int64_t quotient = r_old / r_new;

    tie(d_old, d_new) = make_tuple(d_new, d_old - quotient * d_new);
    tie(r_old, r_new) = make_tuple(r_new, r_old - quotient * r_new);
  }

  // If d is negative, add phi to get modulo
  while (d_old < 0)
  {
    d_old += phi;
  }

  return d_old;
}

// Algorithm from https://en.wikipedia.org/wiki/Modular_exponentiation
int64_t modExp(int64_t base, int64_t exponent, int64_t modulus)
{
  if (modulus == 1)
    return 0;

  int64_t result = 1;
  base %= modulus;
  while (exponent > 0)
  {
    // Check if exponent is odd via MSB
    if (exponent & 1)
      result = (result * base) % modulus;

    // Square the base
    base = (base * base) % modulus;

    // Exp div 2 after square base
    exponent >>= 1;
  }
  return result;
}

vector<char> remapToAlphabet(const std::vector<int64_t> &msg, int64_t shift)
{
  map<int64_t, char> char_map;

  // Define the mapping ordered pair (shift + alphabetical_position, character)
  for (int64_t i = 0; i < 26; ++i)
  {
    char_map[shift + i] = 'A' + i;
  }
  char_map[shift + 26] = ' ';  // blank
  char_map[shift + 27] = '"';  // quotes
  char_map[shift + 28] = ',';  // comma
  char_map[shift + 29] = '.';  // period
  char_map[shift + 30] = '\''; // apostrophe

  std::vector<char> remap_vector;

  for (int64_t num : msg)
  {
    remap_vector.push_back(char_map[num]);
  }

  return remap_vector;
}

int main()
{
  int64_t N = 5353;
  int64_t e = 2397;

  int64_t p, q;

  int64_t msg_length = 0;

  cin >> e;
  cin >> N;

  cin >> msg_length;
  vector<int64_t> msg(msg_length);

  // Input message
  for (int64_t i = 0; i < msg_length; i++)
  {
    cin >> msg[i];
  }

  // int64_t totient = phi(N)[N];
  int64_t totient = totient_naive(N);
  int64_t sum_p_q = N + 1 - totient;

  if (gcd(e, totient) != 1)
  {
    cout << "Public key is not valid!";
    return 0;
  }

  // Solve for p, q from totient(n)
  solveQuadratic(1, -sum_p_q, N, p, q);

  // Swap p,q if p larger q
  if (p > q)
    swap(p, q);

  if (p == q)
  {
    cout << "Public key is not valid!";
    return 0;
  }

  if (!isPrime(p) || !isPrime(q))
  {
    cout << "Public key is not valid!";
    return 0;
  }

  // Solve for decryption exponent
  int64_t d = gcdExtended(e, totient);

  cout << p << " ";
  cout << q << " ";
  cout << totient << " ";
  cout << d << endl;

  // Decrypt message using d and N, store into vector
  vector<int64_t> decrypted_msg(msg_length);

  for (int64_t i = 0; i < msg_length; i++)
  {
    cout << modExp(msg[i], d, N) << " ";
    decrypted_msg[i] = modExp(msg[i], d, N);
  }

  cout << endl;

  // Remap to alphabet and print
  for (char c : remapToAlphabet(decrypted_msg, 7))
  {
    cout << c;
  }

  return 0;
}
