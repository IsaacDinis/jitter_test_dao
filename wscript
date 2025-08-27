#! /usr/bin/env python
# encoding: utf-8
# Sylvain Cetre
import os
import glob
# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='hrtc'

# if you want to cross-compile, use a different command line:
# CC=mingw-gcc AR=mingw-ar waf configure build

top = '.'

from waflib import Configure, Logs, Utils, Context
#Configure.autoconfig = True # True/False/'clobber'

def options(opt):
	opt.load('compiler_c compiler_cxx gnu_dirs')

def configure(conf):
	conf.load('compiler_c compiler_cxx gnu_dirs')
	conf.write_config_header('config.h')
	print('→ prefix is ' + conf.options.prefix)

	conf.check_cfg( package='protobuf',
				args='--cflags --libs',
				uselib_store='PROTOBUF'
				)
	# Check for ZeroMQ
	conf.check_cfg(package='libzmq',
				args='--cflags --libs',
				uselib_store='ZMQ'
				)
	# Enable draft API support
	conf.env.CFLAGS += ['-DZMQ_BUILD_DRAFT_API']
	conf.env.CXXFLAGS += ['-DZMQ_BUILD_DRAFT_API']

	# Check for CUDA
	conf.env.CUDA_AVAILABLE = False  # Default to False
	try:
		nvcc_path = conf.find_program('nvcc', var='NVCC')
		if isinstance(nvcc_path, list):
			nvcc = nvcc_path[0]
		else:
			nvcc = nvcc_path
		conf.env.CUDA_PATH = os.path.dirname(os.path.dirname(nvcc))
		conf.env.INCLUDES_CUDA = [os.path.join(conf.env.CUDA_PATH, 'include')]
		conf.env.LIBPATH_CUDA = [os.path.join(conf.env.CUDA_PATH, 'lib64')]
		conf.env.LIB_CUDA = ['cudart']

		# Check that cuda_runtime.h exists
		conf.check(header_name='cuda_runtime.h', includes=conf.env.INCLUDES_CUDA)
		# Check that cudart lib is there
		conf.check_cxx(lib='cudart', libpath=conf.env.LIBPATH_CUDA)

		conf.env.CUDA_AVAILABLE = True
		print('CUDA detected: enabling GPU build.')
	except Exception as e:
		print('CUDA not found or incomplete: skipping GPU.', e)

	# Check for BLAS
	conf.env.BLAS_AVAILABLE = False  # Default to False
	try:
		conf.check_cfg(package='blas', args='--cflags --libs', uselib_store='BLAS')
		conf.env.BLAS_AVAILABLE = True
		print("BLAS detected: enabling BLAS build.")
	except:
		print("BLAS not found: skipping BLAS.")

def build(bld):
	bld.env.DEFINES=['WAF=1']
	bld.recurse('apps')

	# apps
	files = glob.glob('apps/*.py')
	for file in files:
		bld.install_files(bld.env.PREFIX+'/bin', file, chmod=0o0755, relative_trick=False)
	# script
	files = glob.glob('scripts/*')
	for file in files:
		bld.install_files(bld.env.PREFIX+'/bin', file, chmod=0o0755, relative_trick=False)

