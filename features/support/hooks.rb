require 'trema/path'

def wait_until_all_pid_files_are_deleted(timeout = 12)
  elapsed = 0
  loop do
    fail 'Failed to clean up remaining processes.' if elapsed > timeout
    break if Dir.glob(File.join(Trema.pid, '*.pid')).empty?
    sleep 0.1
    elapsed += 0.1
  end
  sleep 1
end

Before do
  @aruba_timeout_seconds = 10
  run 'trema killall'
  wait_until_all_pid_files_are_deleted
end

Before('@slow_process') do
  @aruba_io_wait_seconds = 1
end

After do
  run 'trema killall'
  wait_until_all_pid_files_are_deleted
  processes.clear
end
