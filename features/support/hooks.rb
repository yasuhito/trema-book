Before do
  @aruba_timeout_seconds = 10
  run 'trema killall'
  sleep 5
end

After do
  run 'trema killall'
  sleep 1
end
