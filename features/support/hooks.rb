Before('@sudo') do
  @aruba_timeout_seconds = 10
  @aruba_io_wait_seconds = 10
end

After('@sudo') do
  run 'trema killall'
end
