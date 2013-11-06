# -*- coding: utf-8 -*-

Before do
  @aruba_timeout_seconds = 10
  run 'trema killall'
  sleep 5
end

After do
  run 'trema killall'
  sleep 1
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
