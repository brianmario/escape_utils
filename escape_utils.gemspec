require './lib/escape_utils/version'

Gem::Specification.new do |s|
  s.name = %q{escape_utils}
  s.version = EscapeUtils::VERSION
  s.authors = ["Brian Lopez"]
  s.date = Time.now.utc.strftime("%Y-%m-%d")
  s.email = %q{seniorlopez@gmail.com}
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = [
    "README.rdoc"
  ]
  s.files = `git ls-files`.split("\n")
  s.homepage = %q{http://github.com/brianmario/escape_utils}
  s.rdoc_options = ["--charset=UTF-8"]
  s.require_paths = ["lib", "ext"]
  s.rubygems_version = %q{1.4.2}
  s.summary = %q{Faster string escaping routines for your web apps}
  s.test_files = `git ls-files spec`.split("\n")

  # tests
  s.add_development_dependency 'rake-compiler', ">= 0.7.5"
  s.add_development_dependency 'rspec', ">= 2.0.0"
  # benchmarks
  s.add_development_dependency 'rack'
  s.add_development_dependency 'haml'
  s.add_development_dependency 'fast_xs'
  s.add_development_dependency 'actionpack'
  s.add_development_dependency 'url_escape'
end

