Before('@sudo') do
  fail 'sudo authentication failed' unless system 'sudo -v'
  @aruba_timeout_seconds = 10
  ENV['TREMA_LOG_DIR'] = '.'
  ENV['TREMA_PID_DIR'] = '.'
  ENV['TREMA_SOCKET_DIR'] = '.'
end

After('@sudo') do
  begin
    run 'trema killall TopologyController'
    sleep 3
  ensure
    ENV['TREMA_LOG_DIR'] = nil
    ENV['TREMA_PID_DIR'] = nil
    ENV['TREMA_SOCKET_DIR'] = nil
  end
end
