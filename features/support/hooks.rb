Before('@sudo') do
  fail 'sudo authentication failed' unless system 'sudo -v'
  @aruba_timeout_seconds = 5
end

After('@sudo') do
  run 'trema killall'
end
