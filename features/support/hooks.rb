Before('@sudo') do
  fail 'sudo authentication failed' unless system 'sudo -v'
  @aruba_timeout_seconds = 30
  @aruba_io_wait_seconds = 30
end

After('@sudo') do
  run 'trema killall'
  sleep 1
end
