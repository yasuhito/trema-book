Before do
  @aruba_timeout_seconds = 5
  run "trema killall"
  sleep 5
end


Before( "@slow_process" ) do
  @aruba_io_wait_seconds = 1
end


After do
  run "trema killall"
  sleep 1
end


### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
