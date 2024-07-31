#!/bin/sh

proxyip=127.0.0.1
socks_port=20170
http_port=20171
phttp="http://${proxyip}:${http_port}"
psocks="socks5://${proxyip}:${socks_port}"

set_proxy() {
  export http_proxy="${phttp}"
  export HTTP_PROXY="${phttp}"

  export https_proxy="${phttp}"
  export HTTPS_PROXY="${phttp}"

  export all_proxy="${psocks}"
  export ALL_PROXY="${psocks}"

  git config --global http.https://github.com.proxy ${phttp}
  git config --global https.https://github.com.proxy ${phttp}

  echo "Proxy has been open."
}

unset_proxy() {
  unset http_proxy
  unset HTTP_PROXY

  unset https_proxy
  unset HTTPS_PROXY

  unset all_proxy
  unset ALL_PROXY

  git config --global --unset http.https://github.com.proxy
  git config --global --unset https.https://github.com.proxy

  echo "Proxy has been closed."
}

test_settings() {
  echo "Try to connect to Google..."
  resp=$(curl -I -s --connect-time 5 -m 5 -w "%{http_code}" -o /dev/null/ www.google.com)
  if [ ${resp} = 200 ]; then
    echo "Proxy setup succeeded!"
  else
    echo "Proxy setup failed!"
  fi
}

if [ "$1" == "set" ]; then
  set_proxy
elif [ "$1" == "unset" ]; then
  unset_proxy
elif [ "$1" == "test" ]; then
  test_settings
else
  echo "Unsupport arguments."
fi
