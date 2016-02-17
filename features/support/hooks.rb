Before('@sudo') do
  raise 'sudo authentication failed' unless system 'sudo -v'
  @aruba_timeout_seconds = 5
end

After('@sudo') do
  run 'trema killall --all -S .'
  sleep 3
end
