Before do
  @aruba_timeout_seconds = 10
end

After do
  run 'trema killall'
end
