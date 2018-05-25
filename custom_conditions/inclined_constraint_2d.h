/*
==============================================================================
KratosStructuralApplication
A library based on:
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi, Janosch Stascheit, Felix Nagel
pooyan@cimne.upc.edu
rrossi@cimne.upc.edu
janosch.stascheit@rub.de
nagel@sd.rub.de
- CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain
- Ruhr-University Bochum, Institute for Structural Mechanics, Germany


Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

The  above  copyright  notice  and  this permission  notice  shall  be
included in all copies or substantial portions of the Software.

THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==============================================================================
*/
/* *********************************************************
*
*   Last Modified by:    $Author: hbui$
*   Date:                $Date: 15 Dec 2017 $
*   Revision:            $Revision: 1.0 $
*
* ***********************************************************/

#if !defined(KRATOS_INCLINED_CONSTRAINT_2D_CONDITION_H_INCLUDED )
#define  KRATOS_INCLINED_CONSTRAINT_2D_CONDITION_H_INCLUDED



// System includes


// External includes
#include "boost/smart_ptr.hpp"


// Project includes
#include "includes/define.h"
#include "includes/condition.h"
#include "includes/serializer.h"
//#include "includes/ublas_interface.h"
#include "includes/variables.h"

namespace Kratos
{
/**
 * Constraint to enforce an inclined boundary condition in 2D using Lagrange multiplier method.
 * The sliding line is defined by equation ax + by + c = 0
 */
class InclinedConstraint2D : public Condition
{

public:
    typedef Condition BaseType;
    typedef BaseType::EquationIdVectorType EquationIdVectorType;
    typedef BaseType::MatrixType LHS_ContributionType;

    typedef Condition::GeometryType::Pointer PointerGeometryType;

    // Counted pointer of PointPointContactLink
    KRATOS_CLASS_POINTER_DEFINITION(InclinedConstraint2D);

    /**
     * Default constructor.
     */
    InclinedConstraint2D( IndexType NewId, GeometryType::Pointer pGeometry);

    InclinedConstraint2D( IndexType NewId, GeometryType::Pointer pGeometry,
                           PropertiesType::Pointer pProperties);

    /**
     * Custom constructor.
     */
    InclinedConstraint2D( IndexType NewId, Node<3>::Pointer const& node,
            const double& a, const double& b, const double& c,
            PropertiesType::Pointer pProperties );

    /**
     * Destructor.
     */
    virtual ~InclinedConstraint2D();

    /**
     * Operations.
     */
    Condition::Pointer Create( IndexType NewId,
                               NodesArrayType const& ThisNodes,
                               PropertiesType::Pointer pProperties) const;


    /**
     * Calculates the local system contributions for this contact element
     */
    void CalculateLocalSystem( MatrixType& rLeftHandSideMatrix,
                               VectorType& rRightHandSideVector,
                               ProcessInfo& rCurrentProcessInfo);

    void CalculateRightHandSide( VectorType& rRightHandSideVector,
                                 ProcessInfo& rCurrentProcessInfo);

    void EquationIdVector( EquationIdVectorType& rResult,
                           ProcessInfo& rCurrentProcessInfo);

    void GetDofList( DofsVectorType& ConditionalDofList,
                     ProcessInfo& CurrentProcessInfo);

private:

    double mA, mB, mC;

    void CalculateAll( MatrixType& rLeftHandSideMatrix,
                       VectorType& rRightHandSideVector,
                       ProcessInfo& rCurrentProcessInfo,
                       bool CalculateStiffnessMatrixFlag,
                       bool CalculateResidualVectorFlag);

    friend class Serializer;

    // A private default constructor necessary for serialization
    InclinedConstraint2D() {};

    virtual void save(Serializer& rSerializer) const
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Condition );
    }

    virtual void load(Serializer& rSerializer)
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Condition );
    }

}; // Class InclinedConstraint2D
}  // namespace Kratos.

#endif // KRATOS_INCLINED_CONSTRAINT_2D_CONDITION_H_INCLUDED  defined

