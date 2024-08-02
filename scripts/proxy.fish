#!/usr/bin/fish

set proxyip 192.168.181.1
set socks_port 10808
set http_port 10809
set phttp "http://$proxyip:$http_port"
set psocks "socks5://$proxyip:$socks_port"

function set_proxy
  export http_proxy="$phttp"
  export HTTP_PROXY="$phttp"

  export https_proxy="$phttp"
  export HTTPS_PROXY="$phttp"

  export all_proxy="$psocks"
  export ALL_PROXY="$psocks"

  git config --global http.https://github.com.proxy $phttp
  git config --global https.https://github.com.proxy $phttp

  echo "Proxy has been open."
end

function unset_proxy
  set -e http_proxy
  set -e HTTP_PROXY

  set -e https_proxy
  set -e HTTPS_PROXY

  set -e all_proxy
  set -e ALL_PROXY

  git config --global --unset http.https://github.com.proxy
  git config --global --unset https.https://github.com.proxy

  echo "Proxy has been closed."
end

function test_settings
  echo "Try to connect to Google..."
  set resp (curl -I -s --connect-time 5 -m 5 -w "%{http_code}" -o /dev/null/ www.google.com)
  if test  $resp -eq 200 
    echo "Proxy setup succeeded!"
  else
    echo "Proxy setup failed!"
  end
end

switch $argv
  case set
    set_proxy
  case unset
    unset_proxy
  case test
    test_settings
  case "*"
    echo "Unsupport arguments."
end
