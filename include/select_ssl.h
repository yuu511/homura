// in order to use SSL in a multithreaded context, we must place mutex callback setups
#include "errlib.h"
#ifdef GNUTLS
#include <gcrypt.h>
#include <errno.h>

GCRY_THREAD_OPTION_PTHREAD_IMPL;

void init_locks(void) 
{
  gcry_control(GCRYCTL_SET_THREAD_CBS);
  DBG("curl with GNUTLS selected\n");
}

#else // the default, openssl
#include <openssl/crypto.h>
#include <deque>
#include <mutex>

std::deque<std::mutex> locks;

static void lock_callback(int mode, int type, char *file, int line) 
{
  (void)file;
  (void)line;
  if(mode & CRYPTO_LOCK) {
    locks[type].lock();
  }
  else {
    locks[type].unlock();
  }
}

static unsigned long thread_id() 
{
  return static_cast<unsigned long> (pthread_self());
}

static void init_locks() 
{
  locks.resize(CRYPTO_num_locks());
  CRYPTO_set_id_callback(&thread_id);
  CRYPTO_set_locking_callback(&lock_callback);
  DBG("curl with OpenSSL selected\n");
}
#endif

