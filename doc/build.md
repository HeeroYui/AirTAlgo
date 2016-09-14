Build lib & build sample                           {#audio_algo_build}
========================

@tableofcontents

Download:                                          {#audio_algo_build_download}
=========

ege use some tools to manage source and build it:

need google repo:                                  {#audio_algo_build_download_repo}
-----------------

see: http://source.android.com/source/downloading.html#installing-repo

On all platform:
```{.sh}
	mkdir ~/.bin
	PATH=~/.bin:$PATH
	curl https://storage.googleapis.com/git-repo-downloads/repo > ~/.bin/repo
	chmod a+x ~/.bin/repo
```

On ubuntu
```{.sh}
	sudo apt-get install repo
```

On archlinux
```{.sh}
	sudo pacman -S repo
```

lutin (build-system):                              {#audio_algo_build_download_lutin}
---------------------

```{.sh}
	pip install lutin --user
	# optionnal dependency of lutin (manage image changing size for application release)
	pip install pillow --user
```


dependency:                                        {#audio_algo_build_download_dependency}
-----------

```{.sh}
	mkdir -p WORKING_DIRECTORY/framework
	cd WORKING_DIRECTORY/framework
	repo init -u git://github.com/atria-soft/manifest.git
	repo sync -j8
	cd ../..
```

sources:                                           {#audio_algo_build_download_sources}
--------

They are already download in the repo manifest in:

```{.sh}
	cd WORKING_DIRECTORY/framework/musicdsp/audio-drain
```

Build:                                             {#audio_algo_build_build}
======

you must stay in your working directory...
```{.sh}
	cd WORKING_DIRECTORY
```

library:                                           {#audio_algo_build_build_library}
--------

```{.sh}
	lutin -mdebug audio-drain
```

Test sample:                                            {#audio_algo_build_build_sample}
------------

```{.sh}
	lutin -mdebug audio-drain-test?build?run
```

