/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date: 2008-02-07 17:17:57 +0100 (Do, 07 Feb 2008) $
Version:   $Revision: 11989 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __mitkTensorImage__h
#define __mitkTensorImage__h

#include "mitkImage.h"
#include "itkVectorImage.h"

#include "MitkDiffusionImagingExports.h"


#define TENSOR_NUM_ELEMENTS 6

namespace mitk 
{

  /**
  * \brief this class encapsulates tensor images
  */
  class MitkDiffusionImaging_EXPORT TensorImage : public Image
  {

  public:

    mitkClassMacro( TensorImage, Image );
    itkNewMacro(Self);
    
    //void CopyConstruct(mitk::Image::Pointer img);

  protected:
    TensorImage();
    virtual ~TensorImage();
  };

} // namespace mitk

#endif /* __mitkTensorImage__h */
