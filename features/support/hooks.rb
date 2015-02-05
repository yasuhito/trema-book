Before do
  @aruba_timeout_seconds = 60
end

After do
  run 'trema killall'
end
