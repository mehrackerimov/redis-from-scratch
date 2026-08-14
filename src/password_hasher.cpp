#include <password_hasher.h>

#include <stdexcept>
#include <sodium.h>


std::string PasswordHasher::hash(const std::string& password) {
    char hash[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(
            hash,
            password.c_str(),
            password.size(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        throw std::runtime_error("Failed to hash password");
    }

    return std::string(hash);
}


bool PasswordHasher::verify(const std::string& password, const std::string& hash) {
    return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.size()) == 0;
}
