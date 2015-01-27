Before do
  @aruba_timeout_seconds = 10
  run 'sudo -v'
end

After do
  run 'trema killall'
end
