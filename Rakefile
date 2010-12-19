# encoding: UTF-8
begin
  require 'jeweler'
  Jeweler::Tasks.new do |gem|
    gem.name = "escape_utils"
    gem.summary = "Faster string escaping routines for your web apps"
    gem.email = "seniorlopez@gmail.com"
    gem.homepage = "http://github.com/brianmario/escape_utils"
    gem.authors = ["Brian Lopez"]
    gem.require_paths = ["lib", "ext"]
    gem.extra_rdoc_files = `git ls-files *.rdoc`.split("\n")
    gem.files = `git ls-files`.split("\n")
    gem.extensions = ["ext/extconf.rb"]
    gem.files.include %w(lib/jeweler/templates/.document lib/jeweler/templates/.gitignore)
    # gem.rubyforge_project = "mysql2"
  end
rescue LoadError
  puts "Jeweler, or one of its dependencies, is not available. Install it with: sudo gem install jeweler -s http://gems.github.com"
end

begin
  require 'rspec'
  require 'rspec/core/rake_task'

  desc "Run all examples with RCov"
  RSpec::Core::RakeTask.new('spec:rcov') do |t|
    t.rcov = true
  end
  RSpec::Core::RakeTask.new('spec') do |t|
    t.verbose = true
  end

  task :default => :spec
rescue LoadError
  puts "rspec, or one of its dependencies, is not available. Install it with: sudo gem install rspec"
end