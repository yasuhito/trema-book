# encoding: utf-8

Before do
  @aruba_timeout_seconds = 10
  run 'trema killall'
end

After do
  run 'trema killall'
  processes.clear
end
