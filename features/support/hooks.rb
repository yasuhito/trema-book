Before('@sudo') do
  fail 'sudo authentication failed' unless system 'sudo -v'
  @aruba_timeout_seconds = 20
end

After('@sudo') do
  run 'trema killall --all'
  sleep 10
end
