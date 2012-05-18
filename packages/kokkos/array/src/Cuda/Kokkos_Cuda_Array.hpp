/*
//@HEADER
// ************************************************************************
// 
//          Kokkos: Node API and Parallel Node Kernels
//              Copyright (2008) Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER
*/

#ifndef KOKKOS_CUDA_ARRAY_HPP
#define KOKKOS_CUDA_ARRAY_HPP

#include <string>

#include <Cuda/Kokkos_Cuda_IndexMap.hpp>

#include <Kokkos_Cuda_macros.hpp>
#include <impl/Kokkos_Array_macros.hpp>
#include <Kokkos_Clear_macros.hpp>

// For the host-mapped memory view:

#include <Kokkos_Host_macros.hpp>
#undef KOKKOS_MACRO_DEVICE
#define KOKKOS_MACRO_DEVICE HostMapped< Cuda >
#include <impl/Kokkos_Array_macros.hpp>
#include <Kokkos_Clear_macros.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace Kokkos {
namespace Impl {

template< typename ArrayType >
struct Factory< Array< ArrayType , Cuda > , void >
{
  typedef Array< ArrayType , Cuda > output_type ;

  static output_type create( const std::string & label , size_t nP )
  {
    output_type array ;
    typedef typename output_type::value_type value_type ;

    array.m_index_map.template assign< value_type >(nP);
    array.m_data.allocate( array.m_index_map.allocation_size() , label );

    // Cuda 'allocate' initializes to zero 

    return array ;
  }
};

template< typename ArrayType >
struct Factory< Array< ArrayType , Cuda > ,
                Array< ArrayType , Cuda > >
{
  typedef Array< ArrayType , Cuda > output_type ;

  static inline
  void deep_copy( const output_type & dst ,
                  const output_type & src )
  {
    typedef typename output_type::value_type value_type ;
    const size_t size = dst.m_index_map.allocation_size() * sizeof(value_type);

    MemoryManager< Cuda >::
      copy_to_device_from_device( dst.m_data.ptr_on_device(),
                                  src.m_data.ptr_on_device(),
                                  size );
  }

  static inline
  output_type create( const output_type & input )
  {
    return Factory< output_type , void >
             ::create( std::string(), input.dimension(0) );
  }
};

/** \brief  The hostview is identically mapped */
template< typename ArrayType >
struct Factory< Array< ArrayType , Cuda > ,
                Array< ArrayType , HostMapped< Cuda > > >
{
  typedef Array< ArrayType , Cuda >                output_type ;
  typedef Array< ArrayType , HostMapped< Cuda > >  input_type ;

  static inline
  void deep_copy( const output_type & output , const input_type & input )
  {
    typedef typename output_type::value_type value_type ;
    const size_t size = output.m_index_map.allocation_size() * sizeof(value_type);

    MemoryManager< Cuda >::
      copy_to_device_from_host( output.m_data.ptr_on_device(),
                                input.m_data.ptr_on_device(),
                                size );
  }
};

template< typename ArrayType >
struct Factory< Array< ArrayType , HostMapped< Cuda > > ,
                Array< ArrayType , Cuda > >
{
  typedef Array< ArrayType , HostMapped< Cuda > > output_type ;
  typedef Array< ArrayType , Cuda >               input_type ;

  static void deep_copy( const output_type & output , const input_type & input )
  {
    typedef typename output_type::value_type value_type ;
    const size_t size = input.m_index_map.allocation_size() * sizeof(value_type);

    MemoryManager< Cuda >::
      copy_to_host_from_device( output.m_data.ptr_on_device(),
                                input.m_data.ptr_on_device(),
                                size );
  }
  static inline
  output_type create( const input_type & input )
  {
    return Factory< output_type , void >
             ::create( std::string(), input.dimension(0) );
  }
};

} // namespace Impl
} // namespace Kokkos

#endif /* #ifndef KOKKOS_CUDA_ARRAY_HPP */
