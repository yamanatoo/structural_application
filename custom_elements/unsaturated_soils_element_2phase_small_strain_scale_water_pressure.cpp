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
*   Last Modified by:    $Author: hbui $
*   Date:                $Date: 2016 Jul 1 $
*   Revision:            $Revision: 1.11 $
*
* ***********************************************************/

// System includes
#include <typeinfo>


// External includes


// Project includes
#include "custom_elements/unsaturated_soils_element_2phase_small_strain_scale_water_pressure.h"
#include "includes/define.h"
#include "utilities/math_utils.h"
#include "custom_utilities/sd_math_utils.h"
#include "geometries/hexahedra_3d_8.h"
#include "geometries/tetrahedra_3d_4.h"
#include "geometries/prism_3d_6.h"
#include "structural_application.h"

//#define ENABLE_DEBUG_CONSTITUTIVE_LAW

namespace Kratos
{

UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure( IndexType NewId,
        GeometryType::Pointer pGeometry )
    : Element( NewId, pGeometry )
{
    //DO NOT ADD DOFS HERE!!!
    mIsInitialized = false;
    mIsStabilised = false;
}

//************************************************************************************
//************************************************************************************
UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure( IndexType NewId,
        GeometryType::Pointer pGeometry,  PropertiesType::Pointer pProperties )
    : Element( NewId, pGeometry, pProperties )
{
    // account for Bezier geometry
    std::string geo_name = typeid(GetGeometry()).name();
    if ( geo_name.find("Bezier") != std::string::npos )
    {
        mNodesPressMin = 1;
        mNodesPressMax = GetGeometry().size();
        mNodesDispMin = 1;
        mNodesDispMax = GetGeometry().size();
        mpPressureGeometry = this->pGetGeometry();
//        mThisIntegrationMethod = GeometryData::GI_GAUSS_1;
        mThisIntegrationMethod = GetGeometry().GetDefaultIntegrationMethod();//default method
        mIsStabilised = true;
        mIsInitialized = false;
        return;
    }

    //setting up the nodal degrees of freedom
    //with DISPLACEMENT_X, DISPLACEMENT_Y, DISPLACEMENT_Z
    //DOFs at the end of time step
    //All calculations are made on the general midpoint alpha
    // Variables DOF_ALPHA are updated in the scheme
    if ( GetGeometry().size() == 27 || GetGeometry().size() == 20 || GetGeometry().size() == 10  || GetGeometry().size() == 15 || GetGeometry().size() == 8 )
    {
        if ( GetGeometry().size() == 27 )
        {
            mNodesPressMin = 1;
            mNodesPressMax = 8;
            mNodesDispMin = 1;
            mNodesDispMax = 27;
            mpPressureGeometry = Geometry< Node<3> >::Pointer( new Hexahedra3D8 <Node<3> >(
                                     GetGeometry()( 0 ), GetGeometry()( 1 ), GetGeometry()( 2 ), GetGeometry()( 3 ),
                                     GetGeometry()( 4 ), GetGeometry()( 5 ), GetGeometry()( 6 ), GetGeometry()( 7 ) ) );
            mThisIntegrationMethod = GeometryData::GI_GAUSS_3;
            mIsStabilised = false;
        }

        if ( GetGeometry().size() == 20 ) //remarks: this element does not work correctly with the 20 nodes discretisation. See the cube consolidation test
        {
            mNodesPressMin = 1;
            mNodesPressMax = 8;
            mNodesDispMin = 1;
            mNodesDispMax = 20;
            mpPressureGeometry = Geometry< Node<3> >::Pointer( new Hexahedra3D8 <Node<3> >(
                                     GetGeometry()( 0 ), GetGeometry()( 1 ), GetGeometry()( 2 ), GetGeometry()( 3 ),
                                     GetGeometry()( 4 ), GetGeometry()( 5 ), GetGeometry()( 6 ), GetGeometry()( 7 ) ) );
            mThisIntegrationMethod = GeometryData::GI_GAUSS_3;
            mIsStabilised = false;
        }

        if ( GetGeometry().size() == 10 )
        {
            mNodesPressMin = 1;
            mNodesPressMax = 4;
            mNodesDispMin = 1;
            mNodesDispMax = 10;
            mpPressureGeometry = Geometry< Node<3> >::Pointer( new Tetrahedra3D4 <Node<3> >( GetGeometry()( 0 ), GetGeometry()( 1 ), GetGeometry()( 2 ), GetGeometry()( 3 ) ) );
//            mThisIntegrationMethod = GeometryData::GI_GAUSS_5; //???
            mThisIntegrationMethod = GeometryData::GI_GAUSS_3; //???
            mIsStabilised = false;
        }

        if ( GetGeometry().size() == 15 )
        {
            mNodesPressMin = 1;
            mNodesPressMax = 6;
            mNodesDispMin = 1;
            mNodesDispMax = 15;
            mpPressureGeometry = Geometry< Node<3> >::Pointer( new Prism3D6 <Node<3> >( GetGeometry()( 0 ), GetGeometry()( 1 ), GetGeometry()( 2 ), GetGeometry()( 3 ), GetGeometry()( 4 ), GetGeometry()( 5 ) ) );
            mThisIntegrationMethod = GeometryData::GI_GAUSS_2;
            mIsStabilised = false;
        }

        // low order element, stabilisation activated
        if ( GetGeometry().size() == 8 )
        {
            mNodesPressMin = 1;
            mNodesPressMax = 8;
            mNodesDispMin = 1;
            mNodesDispMax = 8;
            mpPressureGeometry = Geometry< Node<3> >::Pointer( new Hexahedra3D8 <Node<3> >(
                                     GetGeometry()( 0 ), GetGeometry()( 1 ), GetGeometry()( 2 ), GetGeometry()( 3 ),
                                     GetGeometry()( 4 ), GetGeometry()( 5 ), GetGeometry()( 6 ), GetGeometry()( 7 ) ) );
            mThisIntegrationMethod = GeometryData::GI_GAUSS_2; //remarks: GI_GAUSS_2 gives better result than GI_GAUSS_3 for tunnel problem
            mIsStabilised = true;
        }

        if ( GetGeometry().size() == 4 )
        {
            mNodesPressMin = 1;
            mNodesPressMax = 4;
            mNodesDispMin = 1;
            mNodesDispMax = 4;
            mpPressureGeometry = Geometry< Node<3> >::Pointer( new Tetrahedra3D4 <Node<3> >(
                                     GetGeometry()( 0 ), GetGeometry()( 1 ), GetGeometry()( 2 ), GetGeometry()( 3 ) ) );
            mThisIntegrationMethod = GeometryData::GI_GAUSS_1;
            mIsStabilised = true;
        }
    }
    else
        KRATOS_THROW_ERROR( std::logic_error, "This element matches only with a quadratic hexahedra (8, 20 or 27), tetrahedra (4, 10) or prism (15) geometry" , *this );

    mIsInitialized = false;
}

Element::Pointer UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::Create( IndexType NewId,
        NodesArrayType const& ThisNodes,  PropertiesType::Pointer pProperties ) const
{
    return Element::Pointer( new UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure( NewId, GetGeometry().Create( ThisNodes ),
                             pProperties ) );
}

Element::Pointer UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::Create( IndexType NewId, GeometryType::Pointer pGeom, PropertiesType::Pointer pProperties ) const
{
    return Element::Pointer( new UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure( NewId, pGeom, pProperties ) );
}

UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::~UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure()
{
}

//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::Initialize()
{
    KRATOS_TRY

    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    if ( mIsInitialized )
    {
        //Set Up Initial displacement for StressFreeActivation of Elements
        mInitialDisp.resize( GetGeometry().size(), dim, false );

        for ( unsigned int node = 0; node < GetGeometry().size(); node++ )
            for ( unsigned int i = 0; i < 3; i++ )
                mInitialDisp( node, i ) = GetGeometry()[node].GetSolutionStepValue( DISPLACEMENT )[i];

        return;
    }

    if(GetProperties().Has( INTEGRATION_ORDER ) == true)
    {
        if(GetProperties()[INTEGRATION_ORDER] == 1)
        {
            mThisIntegrationMethod = GeometryData::GI_GAUSS_1;
        }
        else if(GetProperties()[INTEGRATION_ORDER] == 2)
        {
            mThisIntegrationMethod = GeometryData::GI_GAUSS_2;
        }
        else if(GetProperties()[INTEGRATION_ORDER] == 3)
        {
            mThisIntegrationMethod = GeometryData::GI_GAUSS_3;
        }
        else if(GetProperties()[INTEGRATION_ORDER] == 4)
        {
            mThisIntegrationMethod = GeometryData::GI_GAUSS_4;
        }
        else if(GetProperties()[INTEGRATION_ORDER] == 5)
        {
            mThisIntegrationMethod = GeometryData::GI_GAUSS_5;
        }
        else
            KRATOS_THROW_ERROR(std::logic_error, "Does not support for more integration points", *this)
    }

    //number of integration points used, mThisIntegrationMethod refers to the
    //integration method defined in the constructor
    const GeometryType::IntegrationPointsArrayType& integration_points = GetGeometry().IntegrationPoints( mThisIntegrationMethod );

    //initializing the Jacobian, the inverse Jacobian and Jacobians determinant in the reference
    // configuration
    mInvJ0.resize( integration_points.size() );

    for ( unsigned int i = 0; i < integration_points.size(); i++ )
    {
        mInvJ0[i].resize( dim, dim, false );
        noalias( mInvJ0[i] ) = ZeroMatrix( dim, dim );
    }

    mDetJ0.resize( integration_points.size() );

    noalias( mDetJ0 ) = ZeroVector( integration_points.size() );

    mTotalDomainInitialSize = 0.0;
    
    GeometryType::JacobiansType J0( integration_points.size() );

    //calculating the Jacobian
    J0 = GetGeometry().Jacobian( J0, mThisIntegrationMethod );

    //calculating the inverse J0
    for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); PointNumber++ )
    {
        //getting informations for integration
        double IntegrationWeight = integration_points[PointNumber].Weight();
        //calculating and storing inverse of the jacobian and the parameters needed
        MathUtils<double>::InvertMatrix( J0[PointNumber], mInvJ0[PointNumber], mDetJ0[PointNumber] );
        //calculating the total area/volume
        mTotalDomainInitialSize += mDetJ0[PointNumber] * IntegrationWeight;
    }

    //Set Up Initial displacement for StressFreeActivation of Elements
    mInitialDisp.resize( GetGeometry().size(), dim, false );

    for ( unsigned int node = 0; node < GetGeometry().size(); node++ )
        for ( unsigned int i = 0; i < 3; i++ )
            mInitialDisp( node, i ) = GetGeometry()[node].GetSolutionStepValue( DISPLACEMENT )[i];

    //Constitutive Law initialisation
    if ( mConstitutiveLawVector.size() != integration_points.size() )
    {
        mConstitutiveLawVector.resize( integration_points.size() );
        mReferencePressures.resize( integration_points.size() );
        InitializeMaterial();
    }

    mIsInitialized = true;

    KRATOS_CATCH( "" )
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::ResetConstitutiveLaw()
{
    KRATOS_TRY

    if ( GetProperties()[CONSTITUTIVE_LAW] != NULL )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); ++i )
        {
            Vector dummy;
//            dummy = row( GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod ), i );
            mConstitutiveLawVector[i]->ResetMaterial( GetProperties(), GetGeometry(),  dummy);
        }
    }

    KRATOS_CATCH( "" )
}


/**
* THIS method is called from the scheme at the start of each solution step
* @param rCurrentProcessInfo
*/
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::InitializeSolutionStep( ProcessInfo& CurrentProcessInfo )
{
    if(mConstitutiveLawVector[0]->Has(CURRENT_STRAIN_VECTOR))
    {
        std::vector<Vector> Values;
        this->GetValueOnIntegrationPoints(CURRENT_STRAIN_VECTOR, Values, CurrentProcessInfo);
        for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
        {
            mConstitutiveLawVector[Point]->SetValue( CURRENT_STRAIN_VECTOR, Values[Point], CurrentProcessInfo );
        }
    }

    for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
    {
        Vector dummy;
//        dummy = row( GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod ), Point );
        mConstitutiveLawVector[Point]->InitializeSolutionStep( GetProperties(), GetGeometry(), dummy, CurrentProcessInfo );
    }
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::InitializeNonLinearIteration( ProcessInfo& CurrentProcessInfo )
{
    if(mConstitutiveLawVector[0]->Has(CURRENT_STRAIN_VECTOR))
    {
        std::vector<Vector> Values;
        this->GetValueOnIntegrationPoints(CURRENT_STRAIN_VECTOR, Values, CurrentProcessInfo);
        for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
        {
            mConstitutiveLawVector[Point]->SetValue( CURRENT_STRAIN_VECTOR, Values[Point], CurrentProcessInfo );
        }
    }

    for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
    {
        Vector dummy;
//        dummy = row( GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod ), Point );
        mConstitutiveLawVector[Point]->InitializeNonLinearIteration( GetProperties(), GetGeometry(), dummy, CurrentProcessInfo );
    }
}

//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateAll( MatrixType& rLeftHandSideMatrix,
        VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo,
        bool CalculateStiffnessMatrixFlag, bool CalculateResidualVectorFlag )
{
    KRATOS_TRY

    unsigned int number_of_nodes_disp = ( mNodesDispMax - mNodesDispMin + 1 );
    unsigned int number_of_nodes_press = ( mNodesPressMax - mNodesPressMin + 1 );
//    unsigned int number_of_nodes = number_of_nodes_disp + number_of_nodes_press;
    unsigned int dim = GetGeometry().WorkingSpaceDimension();
    unsigned int strain_size = dim * (dim + 1) / 2;

    //resizing as needed the LHS
    unsigned int MatSize1 = ( number_of_nodes_disp * dim + number_of_nodes_press );
    unsigned int MatSizeU = number_of_nodes_disp * dim;
    unsigned int MatSizeP = number_of_nodes_press;

    if ( CalculateStiffnessMatrixFlag == true ) //calculation of the matrix is required
    {
        if ( rLeftHandSideMatrix.size1() != MatSize1 )
            rLeftHandSideMatrix.resize( MatSize1, MatSize1, false );

        noalias( rLeftHandSideMatrix ) = ZeroMatrix( MatSize1, MatSize1 ); //resetting LHS
    }

    //resizing as needed the RHS
    if ( CalculateResidualVectorFlag == true ) //calculation of the matrix is required
    {
        if ( rRightHandSideVector.size() != MatSize1 )
            rRightHandSideVector.resize( MatSize1, false );

        noalias( rRightHandSideVector ) = ZeroVector( MatSize1 ); //resetting RHS
    }

    #ifdef ENABLE_BEZIER_GEOMETRY
    // initialize the geometry
    GetGeometry().Initialize(mThisIntegrationMethod);
    #endif

    //reading integration points and local gradients
    const GeometryType::IntegrationPointsArrayType& integration_points = GetGeometry().IntegrationPoints( mThisIntegrationMethod );

    const GeometryType::ShapeFunctionsGradientsType& DN_De_Displacement =
        GetGeometry().ShapeFunctionsLocalGradients( mThisIntegrationMethod );

    const GeometryType::ShapeFunctionsGradientsType& DN_De_Pressure =
        mpPressureGeometry->ShapeFunctionsLocalGradients( mThisIntegrationMethod );

    const Matrix& Ncontainer_Displacement = GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod );

    const Matrix& Ncontainer_Pressure = mpPressureGeometry->ShapeFunctionsValues( mThisIntegrationMethod );

    double Weight;

    double capillaryPressure;

    double capillaryPressure_Dt;

    double waterPressure;

    double porosity;

    double density;

    double DetJ = 0.0;

    Matrix Help_K_UU( MatSizeU, MatSizeU );

    Matrix Help_K_UW( MatSizeU, MatSizeP );

    Matrix Help_K_WU( MatSizeP, MatSizeU );

    Matrix Help_K_WW( MatSizeP, MatSizeP );

    Vector Help_R_U( MatSizeU );

    Vector Help_R_W( MatSizeP );

    Matrix DN_DX_PRESS( number_of_nodes_press, 3 );

    Vector N_DISP( number_of_nodes_disp );

    Vector N_PRESS( number_of_nodes_press );

    Matrix tanC_W( dim, dim );

    if ( CalculateStiffnessMatrixFlag == true ) //calculation of the matrix is required
    {
        noalias( Help_K_UU ) = ZeroMatrix( MatSizeU, MatSizeU );
        noalias( Help_K_UW ) = ZeroMatrix( MatSizeU, MatSizeP );

        noalias( Help_K_WU ) = ZeroMatrix( MatSizeP, MatSizeU );
        noalias( Help_K_WW ) = ZeroMatrix( MatSizeP, MatSizeP );
    }

    if ( CalculateResidualVectorFlag == true ) //calculation of the matrix is required
    {
        noalias( Help_R_U ) = ZeroVector( MatSizeU );
        noalias( Help_R_W ) = ZeroVector( MatSizeP );
    }

    //Initialize local variables
    Matrix B( strain_size, MatSizeU );

    Matrix TanC_U( strain_size, strain_size );

    Vector StrainVector( strain_size );

    Vector StressVector( strain_size );

    Matrix DN_DX_DISP( number_of_nodes_disp, dim );

    Matrix CurrentDisp( number_of_nodes_disp, dim );

    //Current displacements
    for ( unsigned int node = 0; node < GetGeometry().size(); ++node )
    {
        CurrentDisp( node, 0 ) = GetGeometry()[node].GetSolutionStepValue( DISPLACEMENT_X );
        CurrentDisp( node, 1 ) = GetGeometry()[node].GetSolutionStepValue( DISPLACEMENT_Y );
        CurrentDisp( node, 2 ) = GetGeometry()[node].GetSolutionStepValue( DISPLACEMENT_Z );
    }

    /////////////////////////////////////////////////////////////////////////
    //// Integration in space to compute the average of pressure shape function
    /////////////////////////////////////////////////////////////////////////
    Vector N_PRESS_averaged( number_of_nodes_press );
    if(mIsStabilised)
    {
        N_PRESS_averaged = ZeroVector( number_of_nodes_press );
        for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); ++PointNumber )
        {
            Weight = integration_points[PointNumber].Weight();
            DetJ   = mDetJ0[PointNumber];

            // Shape Functions on current spatial quadrature point
            noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );

            noalias( N_PRESS_averaged ) += N_PRESS * Weight * DetJ;
        }
        N_PRESS_averaged /= mTotalDomainInitialSize;
    }

    /////////////////////////////////////////////////////////////////////////
    //// Integration in space to compute the average of capillaryPressure_Dt
    /////////////////////////////////////////////////////////////////////////
    double averageCapillaryPressure_dt = 0.0;
    if(mIsStabilised)
    {
        for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); ++PointNumber )
        {
            Weight = integration_points[PointNumber].Weight();
            DetJ   = mDetJ0[PointNumber];

            // Shape Functions on current spatial quadrature point
            noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );
            capillaryPressure_Dt = GetDerivativeDCapillaryPressureDt(N_PRESS);

            averageCapillaryPressure_dt += capillaryPressure_Dt * Weight * DetJ;
        }
        averageCapillaryPressure_dt /= mTotalDomainInitialSize;
    }

    /////////////////////////////////////////////////////////////////////////
    //// Compute the B-dilatational operator
    //// Reference: Thomas Hughes, The Finite Element Method
    /////////////////////////////////////////////////////////////////////////
    Matrix Bdil_bar;
    if(GetProperties().Has(IS_BBAR))
    {
        if(GetProperties()[IS_BBAR] == true)
        {
            Bdil_bar.resize(number_of_nodes_disp, dim);
            noalias(Bdil_bar) = ZeroMatrix(number_of_nodes_disp, dim);
            for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); ++PointNumber )
            {
                Weight = integration_points[PointNumber].Weight();
                DetJ   = mDetJ0[PointNumber];
                noalias( DN_DX_DISP ) = prod( DN_De_Displacement[PointNumber], mInvJ0[PointNumber] );
//                KRATOS_WATCH(DN_DX_DISP)
//                KRATOS_WATCH(Weight)
//                KRATOS_WATCH(DetJ)
                noalias(Bdil_bar) += DN_DX_DISP * Weight * DetJ;
            }
            Bdil_bar /= mTotalDomainInitialSize;
        }
    }
//    KRATOS_WATCH(Bdil_bar / dim)
//    KRATOS_WATCH(mTotalDomainInitialSize)
    /////////////////////////////////////////////////////////////////////////
    //// Integration in space sum_(beta=0)^(number of quadrature points) ////
    /////////////////////////////////////////////////////////////////////////
    for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); ++PointNumber )
    {
        noalias( DN_DX_PRESS ) = prod( DN_De_Pressure[PointNumber], mInvJ0[PointNumber] );

        noalias( DN_DX_DISP ) = prod( DN_De_Displacement[PointNumber], mInvJ0[PointNumber] );

        Weight = integration_points[PointNumber].Weight();

        DetJ = mDetJ0[PointNumber];

        // Shape Functions on current spatial quadrature point
        noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );
        noalias( N_DISP ) = row( Ncontainer_Displacement, PointNumber );

        //Initializing B_Operator at the current integration point
        if(GetProperties().Has(IS_BBAR))
        {
            if(GetProperties()[IS_BBAR] == true)
                CalculateBBaroperator( B, DN_DX_DISP, Bdil_bar );
            else
                CalculateBoperator( B, DN_DX_DISP );
        }
        else
            CalculateBoperator( B, DN_DX_DISP );

        //Calculate the current strain vector using the B-Operator
        CalculateStrain( B, CurrentDisp, StrainVector );

        noalias(StressVector) = ZeroVector(6);

        double waterPressure_scale = rCurrentProcessInfo[WATER_PRESSURE_SCALE];
        GetPressures( N_PRESS, capillaryPressure, waterPressure, waterPressure_scale );
        // REMARKS: comment this to maintain consistent linearisation
//        if( waterPressure < 0.0 ) // avoid going into saturated state
//        {
//            waterPressure = 0.0;
//        }

        porosity = GetPorosity( DN_DX_DISP );

        if ( porosity > 1.0 || porosity < 0.0 )
        {
            KRATOS_THROW_ERROR(std::logic_error, "porosity is ", porosity)
        }

        density = GetAveragedDensity( capillaryPressure/rCurrentProcessInfo[WATER_PRESSURE_SCALE], porosity );

        CalculateStressAndTangentialStiffnessUnsaturatedSoils( StressVector, TanC_U, tanC_W, StrainVector, waterPressure,  PointNumber, rCurrentProcessInfo );

        if ( CalculateStiffnessMatrixFlag == true )
        {
            //Calculation of spatial Stiffnes and Mass Matrix
            CalculateStiffnesMatrixUU( Help_K_UU, TanC_U, B, DN_DX_DISP, N_DISP, density,  capillaryPressure, Weight, DetJ, waterPressure_scale );
            CalculateStiffnesMatrixUW( Help_K_UW, tanC_W, DN_DX_DISP, N_DISP, N_PRESS, capillaryPressure, Weight, DetJ, waterPressure_scale );
            CalculateStiffnesMatrixWU( Help_K_WU, DN_DX_DISP, DN_DX_PRESS, N_PRESS, capillaryPressure,  Weight, DetJ );
            CalculateStiffnesMatrixWW( Help_K_WW, DN_DX_DISP, DN_DX_PRESS, N_PRESS, capillaryPressure, Weight, DetJ );
            //Calculate stabilisation term
            if(mIsStabilised)
                CalculateStiffnesMatrixWWs( Help_K_WW, N_PRESS, N_PRESS_averaged, Weight, DetJ );
        }

        if ( CalculateResidualVectorFlag == true )
        {
            //Calculation of spatial Loadvector
            AddBodyForcesToRHSVectorU( Help_R_U, N_DISP, density, Weight, DetJ );
            AddInternalForcesToRHSU( Help_R_U, B, StressVector, Weight, DetJ );
            AddInternalForcesToRHSW( Help_R_W, DN_DX_DISP, DN_DX_PRESS, N_PRESS, capillaryPressure, Weight, DetJ );
            if(mIsStabilised)
            {
                capillaryPressure_Dt = GetDerivativeDCapillaryPressureDt(N_PRESS);
                AddInternalForcesToRHSWs( Help_R_W, N_PRESS, N_PRESS_averaged, capillaryPressure_Dt, averageCapillaryPressure_dt, Weight, DetJ );
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////
    // END Integration in space sum_(beta=0)^(number of quadrature points)
    ///////////////////////////////////////////////////////////////////////

    if ( CalculateStiffnessMatrixFlag == true )
    {
        AssembleTimeSpaceStiffnessFromStiffSubMatrices( rLeftHandSideMatrix, Help_K_UU, Help_K_UW, Help_K_WU, Help_K_WW );
    }

    if ( CalculateResidualVectorFlag == true )
    {
        AssembleTimeSpaceRHSFromSubVectors( rRightHandSideVector, Help_R_U, Help_R_W );
    }

    #ifdef ENABLE_BEZIER_GEOMETRY
    // finalize the geometry
    GetGeometry().Clean();
    #endif

//    KRATOS_WATCH(Help_R_U)
//    KRATOS_WATCH(Help_R_W)
//    KRATOS_WATCH(Help_K_UU)
//    KRATOS_WATCH(Help_K_UW)
//    KRATOS_WATCH(Help_K_WU)
//    KRATOS_WATCH(Help_K_WW)

//    KRATOS_WATCH(rRightHandSideVector)
//    KRATOS_WATCH(rLeftHandSideMatrix)

    KRATOS_CATCH( "" )
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateRightHandSide( VectorType& rRightHandSideVector,
        ProcessInfo& rCurrentProcessInfo )
{
    //calculation flags
    bool CalculateStiffnessMatrixFlag = false;
    bool CalculateResidualVectorFlag = true;
    MatrixType temp = Matrix();

    CalculateAll( temp, rRightHandSideVector, rCurrentProcessInfo, CalculateStiffnessMatrixFlag,  CalculateResidualVectorFlag );
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateLocalSystem( MatrixType& rLeftHandSideMatrix,
        VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo )
{
    //calculation flags
    bool CalculateStiffnessMatrixFlag = true;
    bool CalculateResidualVectorFlag = true;
    CalculateAll( rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo,
                  CalculateStiffnessMatrixFlag, CalculateResidualVectorFlag );
}

////************************************************************************************
////************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::MassMatrix(MatrixType& rMassMatrix, ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_THROW_ERROR(std::logic_error, "Deprecated method", "")
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateMassMatrix( MatrixType& rMassMatrix, ProcessInfo& rCurrentProcessInfo )
{
    CalculateDampingMatrix(rMassMatrix, rCurrentProcessInfo);
}

////************************************************************************************
////************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::DampMatrix(MatrixType& rDampMatrix, ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_THROW_ERROR(std::logic_error, "Deprecated method", "")
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateDampingMatrix( MatrixType& rDampMatrix, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    unsigned int number_of_nodes_disp = ( mNodesDispMax - mNodesDispMin + 1 );
    unsigned int number_of_nodes_press = ( mNodesPressMax - mNodesPressMin + 1 );
//    unsigned int number_of_nodes = number_of_nodes_disp+number_of_nodes_press;
    unsigned int dim = GetGeometry().WorkingSpaceDimension();
    //resizing as needed the LHS
    unsigned int MatSize1 = ( number_of_nodes_disp * dim + number_of_nodes_press );
    unsigned int MatSizeU = number_of_nodes_disp * dim;
    unsigned int MatSizeP = number_of_nodes_press;

    if ( rDampMatrix.size1() != MatSize1 )
        rDampMatrix.resize( MatSize1, MatSize1 );

    noalias( rDampMatrix ) = ZeroMatrix( MatSize1, MatSize1 ); //resetting LHS

    #ifdef ENABLE_BEZIER_GEOMETRY
    // initialize the geometry
    GetGeometry().Initialize(mThisIntegrationMethod);
    #endif

    const Matrix& Ncontainer_Pressure = mpPressureGeometry->ShapeFunctionsValues( mThisIntegrationMethod );

    //auxiliary terms
//         InitializeGalerkinScheme(rCurrentProcessInfo);

    double Weight;

    //reading integration points and local gradients
    const GeometryType::IntegrationPointsArrayType& integration_points = GetGeometry().IntegrationPoints( mThisIntegrationMethod );

    const GeometryType::ShapeFunctionsGradientsType& DN_De_Displacement =
        GetGeometry().ShapeFunctionsLocalGradients( mThisIntegrationMethod );

    double capillaryPressure;

    double waterPressure;

    Matrix Help_D_UU( MatSizeU, MatSizeU );

    Matrix Help_D_UW( MatSizeU, MatSizeP );

    Matrix Help_D_WU( MatSizeP, MatSizeU );

    Matrix Help_D_WW( MatSizeP, MatSizeP );

    Matrix DN_DX_DISP( number_of_nodes_disp, 3 );

    Vector N_PRESS( number_of_nodes_press );

    noalias( Help_D_UU ) = ZeroMatrix( MatSizeU, MatSizeU );

    noalias( Help_D_UW ) = ZeroMatrix( MatSizeU, MatSizeP );

    noalias( Help_D_WU ) = ZeroMatrix( MatSizeP, MatSizeU );

    noalias( Help_D_WW ) = ZeroMatrix( MatSizeP, MatSizeP );

    double DetJ = 0.0;

    /////////////////////////////////////////////////////////////////////////
    //// Integration in space to compute the average of pressure shape function
    /////////////////////////////////////////////////////////////////////////
    Vector N_PRESS_averaged( number_of_nodes_press );
    if(mIsStabilised)
    {
        N_PRESS_averaged = ZeroVector( number_of_nodes_press );
        for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); ++PointNumber )
        {
            Weight = integration_points[PointNumber].Weight();
            DetJ   = mDetJ0[PointNumber];

            // Shape Functions on current spatial quadrature point
            noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );

            noalias( N_PRESS_averaged ) += N_PRESS * Weight * DetJ;
        }
        N_PRESS_averaged /= mTotalDomainInitialSize;
    }

    /////////////////////////////////////////////////////////////////////////
    //// Integration in space sum_(beta=0)^(number of quadrature points)
    /////////////////////////////////////////////////////////////////////////
    for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); PointNumber++ )
    {
        noalias( DN_DX_DISP ) = prod( DN_De_Displacement[PointNumber], mInvJ0[PointNumber] );

//     double DetDef= Determinant_DeformationTensor(DN_DX_DISP);

        Weight = integration_points[PointNumber].Weight();
        // Jacobian on current quadrature point

        DetJ = mDetJ0[PointNumber];
        // Shape Functions on current spatial quadrature point
        noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );

        GetPressures( N_PRESS, capillaryPressure, waterPressure, rCurrentProcessInfo[WATER_PRESSURE_SCALE] );
//        KRATOS_WATCH(waterPressure);

//                         Calculation of spatial Stiffnes and Mass Matrix
        CalculateDampingMatrixWU( Help_D_WU, DN_DX_DISP, N_PRESS,
                                  capillaryPressure, Weight, DetJ );
        CalculateDampingMatrixWW( Help_D_WW, DN_DX_DISP, N_PRESS,
                                  capillaryPressure, Weight, DetJ );
        if(mIsStabilised)
            CalculateDampingMatrixWWs( Help_D_WW, DN_DX_DISP, N_PRESS,
                                      N_PRESS_averaged, Weight, DetJ );
//        CalculateMassMatrix(HelpMassMatrix, N, Weight,DetJ,density);
    }
    ///////////////////////////////////////////////////////////////////////
    // END Integration in space sum_(beta=0)^(number of quadrature points)
    ///////////////////////////////////////////////////////////////////////

    AssembleTimeSpaceStiffnessFromDampSubMatrices( rDampMatrix, Help_D_UU, Help_D_UW, Help_D_WU, Help_D_WW );

    #ifdef ENABLE_BEZIER_GEOMETRY
    // finalize the geometry
    GetGeometry().Clean();
    #endif

    KRATOS_CATCH( "" )
}

////************************************************************************************
////************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::FinalizeNonLinearIteration( ProcessInfo& CurrentProcessInfo )
{
    if(mConstitutiveLawVector[0]->Has(CURRENT_STRAIN_VECTOR))
    {
        std::vector<Vector> Values;
        this->GetValueOnIntegrationPoints(CURRENT_STRAIN_VECTOR, Values, CurrentProcessInfo);
        for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
        {
            mConstitutiveLawVector[Point]->SetValue( CURRENT_STRAIN_VECTOR, Values[Point], CurrentProcessInfo );
        }
    }

    for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
    {
        Vector dummy;
//        dummy = row( GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod ), Point );
        mConstitutiveLawVector[Point]->FinalizeNonLinearIteration( GetProperties(), GetGeometry(), dummy, CurrentProcessInfo );
    }
}

////************************************************************************************
////************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::FinalizeSolutionStep( ProcessInfo& CurrentProcessInfo )
{
    if(mConstitutiveLawVector[0]->Has(CURRENT_STRAIN_VECTOR))
    {
        std::vector<Vector> Values;
        this->GetValueOnIntegrationPoints(CURRENT_STRAIN_VECTOR, Values, CurrentProcessInfo);
        for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
        {
            mConstitutiveLawVector[Point]->SetValue( CURRENT_STRAIN_VECTOR, Values[Point], CurrentProcessInfo );
        }
    }

    for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); ++Point )
    {
        Vector dummy;
//        dummy = row( GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod ), Point );
        mConstitutiveLawVector[Point]->FinalizeSolutionStep( GetProperties(), GetGeometry(), dummy, CurrentProcessInfo );
    }

//    Vector Dummy_Vector( 9 );
// 
//    noalias( Dummy_Vector ) = mConstitutiveLawVector[0]->GetValue( INTERNAL_VARIABLES, Dummy_Vector );
// 
//    for ( unsigned int i = 0; i < GetGeometry().size(); i++ )
//    {
//        GetGeometry()[i].GetSolutionStepValue( MOMENTUM_X ) = Dummy_Vector( 0 );
//        GetGeometry()[i].GetSolutionStepValue( MOMENTUM_Y ) = Dummy_Vector( 1 );
//        GetGeometry()[i].GetSolutionStepValue( MOMENTUM_Z ) = Dummy_Vector( 2 );
//        GetGeometry()[i].GetSolutionStepValue( PRESSURE ) = Dummy_Vector( 3 );
//        GetGeometry()[i].GetSolutionStepValue( ERROR_RATIO ) = Dummy_Vector( 4 );
//        GetGeometry()[i].GetSolutionStepValue( EXCESS_PORE_WATER_PRESSURE ) = GetGeometry()[i].GetSolutionStepValue( WATER_PRESSURE ) - 9.81 * 1000.0 * ( 20.0 - GetGeometry()[i].Z() );
//    }
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateOnIntegrationPoints( const Variable<double >& rVariable, std::vector<double>& Output, const ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    unsigned int number_of_nodes_press = ( mNodesPressMax - mNodesPressMin + 1 );

    #ifdef ENABLE_BEZIER_GEOMETRY
    // initialize the geometry
    GetGeometry().Initialize(mThisIntegrationMethod);
    #endif

    //reading integration points and local gradients
    const GeometryType::IntegrationPointsArrayType& integration_points = GetGeometry().IntegrationPoints( mThisIntegrationMethod );

    if ( Output.size() != integration_points.size() )
        Output.resize( integration_points.size() );

    const Matrix& Ncontainer_Pressure = mpPressureGeometry->ShapeFunctionsValues( mThisIntegrationMethod );

    Vector N_PRESS( number_of_nodes_press );

    double capillaryPressure;

    double waterPressure;

    double saturation;

    /////////////////////////////////////////////////////////////////////////
    //// Integration in space sum_(beta=0)^(number of quadrature points)
    /////////////////////////////////////////////////////////////////////////
    for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); PointNumber++ )
    {
        // Shape Functions on current spatial quadrature point
        if ( N_PRESS.size() != number_of_nodes_press )
            N_PRESS.resize( number_of_nodes_press );

        noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );
        GeometryType::CoordinatesArrayType gp_position;
        gp_position = GetGeometry().GlobalCoordinates( gp_position, integration_points[PointNumber] );

        GetPressures( N_PRESS, capillaryPressure, waterPressure, rCurrentProcessInfo[WATER_PRESSURE_SCALE] );

        saturation = GetSaturation( capillaryPressure );

        if ( rVariable == SATURATION )
        {
            Output[PointNumber] = saturation;
        }

        if ( rVariable == WATER_PRESSURE )
        {
            Output[PointNumber] = waterPressure;
        }

        if ( rVariable == EXCESS_PORE_WATER_PRESSURE )
        {
            Output[PointNumber] = waterPressure - mReferencePressures[PointNumber];
        }

        if ( rVariable == AIR_PRESSURE )
        {
            Output[PointNumber] = 0.0;
        }
    /////////////////////////////////////////////////////////////////////////
    //// End Integration in space sum_(beta=0)^(number of quadrature points)
    /////////////////////////////////////////////////////////////////////////
    }

    #ifdef ENABLE_BEZIER_GEOMETRY
    // finalize the geometry
    GetGeometry().Clean();
    #endif

    KRATOS_CATCH( "" )
}

/**
* Calculate Vector Variables at each integration point, used for postprocessing etc.
* @param rVariable Global name of the variable to be calculated
* @param output Vector to store the values on the qudrature points, output of the method
* @param rCurrentProcessInfo
*/
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateOnIntegrationPoints( const Variable<Vector>& rVariable,
        std::vector<Vector>& Output, const ProcessInfo& rCurrentProcessInfo )
{
    GetValueOnIntegrationPoints( rVariable, Output, rCurrentProcessInfo );
}


//************************************************************************************
//************************************************************************************

inline void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateAndAddExtForceContribution( const Vector& N, const ProcessInfo& CurrentProcessInfo, Vector& BodyForce, VectorType& rRightHandSideVector, double weight )
{
    KRATOS_TRY

    KRATOS_CATCH( "" )
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::EquationIdVector( EquationIdVectorType& rResult,
        ProcessInfo& CurrentProcessInfo )
{
    DofsVectorType ElementalDofList;
    this->GetDofList(ElementalDofList, CurrentProcessInfo);
    rResult.resize(ElementalDofList.size());
    for(unsigned int i = 0; i < ElementalDofList.size(); ++i)
        rResult[i] = ElementalDofList[i]->EquationId();
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDofList( DofsVectorType& ElementalDofList, ProcessInfo&
        CurrentProcessInfo )
{
    ElementalDofList.resize( 0 );

    for ( unsigned int i = ( mNodesPressMin - 1 ); i < mNodesPressMax; i++ )
    {
        ElementalDofList.push_back( GetGeometry()[i].pGetDof( WATER_PRESSURE ) );
    }

    for ( unsigned int i = ( mNodesDispMin - 1 ); i < mNodesDispMax; i++ )
    {
        ElementalDofList.push_back( GetGeometry()[i].pGetDof( DISPLACEMENT_X ) );
        ElementalDofList.push_back( GetGeometry()[i].pGetDof( DISPLACEMENT_Y ) );
        ElementalDofList.push_back( GetGeometry()[i].pGetDof( DISPLACEMENT_Z ) );
    }
}

//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetValuesVector( Vector& values, int Step )
{
    unsigned int dim_press = 1;//two pressure dofs
    unsigned int dim_disp = ( GetGeometry().WorkingSpaceDimension() );//3 displacement dofs
    unsigned int MatSize =
        ( mNodesPressMax - mNodesPressMin + 1 ) * dim_press + ( mNodesDispMax -
                mNodesDispMin + 1 ) * dim_disp;

    if ( values.size() != MatSize )
        values.resize( MatSize );

    unsigned int adddisp = ( mNodesPressMax - mNodesPressMin + 1 ) * dim_press;

    for ( unsigned int i = ( mNodesPressMin - 1 ); i < mNodesPressMax; i++ )
    {
        int index = ( i - mNodesPressMin + 1 ) * dim_press;
        values( index ) =
            GetGeometry()[i].GetSolutionStepValue( WATER_PRESSURE, Step );
    }

    for ( unsigned int i = ( mNodesDispMin - 1 ); i < mNodesDispMax; i++ )
    {
        int index = adddisp + ( i - mNodesDispMin + 1 ) * dim_disp;

        values( index ) =
            GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT_X, Step );
        values( index + 1 ) =
            GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT_Y, Step );
        values( index + 2 ) =
            GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT_Z, Step );
    }

}

//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::AssembleTimeSpaceStiffnessFromDampSubMatrices
( MatrixType& rLeftHandSideMatrix, const Matrix& D_UU, const Matrix& D_UW,
  const Matrix&  D_WU, const Matrix&  D_WW )
{

    unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    unsigned int number_of_nodes_disp = mNodesDispMax - mNodesDispMin + 1;
    unsigned int number_of_nodes_press = mNodesPressMax - mNodesPressMin + 1;
    unsigned int dim_disp = dimension;
    unsigned int dim_press = 1;
    unsigned int index_time_prim;
    unsigned int index_time_sec;
    unsigned int index_space_prim;
    unsigned int index_space_sec;
    unsigned int addIndex_disp = number_of_nodes_press * dim_press;

    for ( unsigned int prim = 0; prim < number_of_nodes_disp; prim++ )
    {
        for ( unsigned int i = 0; i < dim_disp; i++ )
        {
            index_space_prim = prim * dim_disp + i;

            index_time_prim = addIndex_disp + prim * dim_disp + i;

            for ( unsigned int sec = 0; sec < number_of_nodes_disp; sec++ )
            {
                for ( unsigned int j = 0; j < dim_disp; j++ )
                {
                    index_space_sec = sec * dim_disp + j;

                    index_time_sec = addIndex_disp + sec * dim_disp + j;

                    rLeftHandSideMatrix( index_time_prim,
                                         index_time_sec )
                    +=
                        ( -1 ) *
                        D_UU( index_space_prim, index_space_sec );
                }
            }
        }
    }

    for ( unsigned int prim = 0; prim < number_of_nodes_disp; prim++ )
    {
        for ( unsigned int i = 0; i < dim_disp; i++ )
        {
            index_space_prim = prim * dim_disp + i;

            index_time_prim = addIndex_disp + prim * dim_disp + i;

            for ( unsigned int sec = 0; sec < number_of_nodes_press; sec++ )
            {
                index_space_sec = sec;

                index_time_sec = sec * dim_press;

                rLeftHandSideMatrix( index_time_prim, index_time_sec )
                +=
                    ( -1 ) *
                    D_UW( index_space_prim, index_space_sec );
            }
        }
    }

    for ( unsigned int prim = 0; prim < number_of_nodes_press; prim++ )
    {

        index_space_prim = prim;

        index_time_prim = prim * dim_press;

        for ( unsigned int sec = 0; sec < number_of_nodes_disp; sec++ )
        {
            for ( unsigned int j = 0; j < dim_disp; j++ )
            {
                index_space_sec = sec * dim_disp + j;

                index_time_sec = addIndex_disp + sec * dim_disp + j;

                rLeftHandSideMatrix( index_time_prim, index_time_sec )
                +=
                    ( -1 ) *
                    D_WU( index_space_prim, index_space_sec );
            }
        }
    }

    for ( unsigned int prim = 0; prim < number_of_nodes_press; prim++ )
    {
        index_space_prim = prim;

        index_time_prim = prim * dim_press;

        for ( unsigned int sec = 0; sec < number_of_nodes_press; sec++ )
        {
            index_space_sec = sec;

            index_time_sec = sec * dim_press;

            rLeftHandSideMatrix( index_time_prim, index_time_sec )
            +=
                ( -1 ) *
                D_WW( index_space_prim, index_space_sec );
        }
    }
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::AssembleTimeSpaceStiffnessFromStiffSubMatrices
( MatrixType& rLeftHandSideMatrix, const Matrix& K_UU, const Matrix& K_UW,
  const Matrix& K_WU, const Matrix& K_WW )
{
    KRATOS_TRY

    unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    unsigned int number_of_nodes_disp = mNodesDispMax - mNodesDispMin + 1;
    unsigned int number_of_nodes_press = mNodesPressMax - mNodesPressMin + 1;
    unsigned int dim_disp = dimension;
    unsigned int dim_press = 1;
    unsigned int index_time_prim;
    unsigned int index_time_sec;
    unsigned int index_space_prim;
    unsigned int index_space_sec;
    unsigned int addIndex_disp = number_of_nodes_press * dim_press;

    for ( unsigned int prim = 0; prim < number_of_nodes_disp; prim++ )
    {
        for ( unsigned int i = 0; i < dim_disp; i++ )
        {
            index_space_prim = prim * dim_disp + i;

            index_time_prim = addIndex_disp + prim * dim_disp + i;

            for ( unsigned int sec = 0; sec < number_of_nodes_disp; sec++ )
            {
                for ( unsigned int j = 0; j < dim_disp; j++ )
                {
                    index_space_sec = sec * dim_disp + j;

                    index_time_sec = addIndex_disp + sec * dim_disp + j;

                    rLeftHandSideMatrix( index_time_prim, index_time_sec )
                    +=
                        ( -1 ) *
                        K_UU( index_space_prim, index_space_sec );
                }
            }
        }
    }

    for ( unsigned int prim = 0; prim < number_of_nodes_disp; prim++ )
    {
        for ( unsigned int i = 0; i < dim_disp; i++ )
        {
            index_space_prim = prim * dim_disp + i;

            index_time_prim = addIndex_disp + prim * dim_disp + i;

            for ( unsigned int sec = 0; sec < number_of_nodes_press; sec++ )
            {
                index_space_sec = sec;

                index_time_sec = sec * dim_press;

                rLeftHandSideMatrix( index_time_prim, index_time_sec )
                +=
                    ( -1 ) *
                    K_UW( index_space_prim, index_space_sec );
            }
        }
    }

    for ( unsigned int prim = 0; prim < number_of_nodes_press; prim++ )
    {

        index_space_prim = prim;

        index_time_prim = prim * dim_press;

        for ( unsigned int sec = 0; sec < number_of_nodes_disp; sec++ )
        {
            for ( unsigned int j = 0; j < dim_disp; j++ )
            {
                index_space_sec = sec * dim_disp + j;

                index_time_sec = addIndex_disp + sec * dim_disp + j;

                rLeftHandSideMatrix( index_time_prim, index_time_sec )
                +=
                    ( -1 ) *
                    K_WU( index_space_prim, index_space_sec );
            }
        }
    }

    for ( unsigned int prim = 0; prim < number_of_nodes_press; prim++ )
    {
        index_space_prim = prim;

        index_time_prim = prim * dim_press;

        for ( unsigned int sec = 0; sec < number_of_nodes_press; sec++ )
        {
            index_space_sec = sec;

            index_time_sec = sec * dim_press;

            rLeftHandSideMatrix( index_time_prim, index_time_sec )
            +=
                ( -1 ) *
                K_WW( index_space_prim, index_space_sec );
        }
    }

    KRATOS_CATCH( "" )
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::AssembleTimeSpaceRHSFromSubVectors( VectorType&
        rRightHandSideVector, const Vector& R_U, const Vector& R_W )
{
    KRATOS_TRY

    unsigned int dimension = GetGeometry().WorkingSpaceDimension();
    unsigned int number_of_nodes_disp = ( mNodesDispMax - mNodesDispMin + 1 );
    unsigned int number_of_nodes_press = ( mNodesPressMax - mNodesPressMin + 1 );
    unsigned int dim_disp = dimension;
    unsigned int dim_press = 1;
    unsigned int index_time;
    unsigned int addIndex_disp = number_of_nodes_press * dim_press;

//    noalias(rRightHandSideVector)= ZeroVector(number_of_nodes_press*dim_1+(number_of_nodes_disp-number_of_nodes_press)*dim_2);

    for ( unsigned int prim = 0; prim < number_of_nodes_disp; prim++ )
    {
        for ( unsigned int i = 0; i < dim_disp; i++ )
        {
            index_time = addIndex_disp + prim * dim_disp + i;

            rRightHandSideVector( index_time ) +=
                R_U( prim * dim_disp + i );
        }
    }

    for ( unsigned int prim = 0; prim < number_of_nodes_press; prim++ )
    {
        index_time = prim * dim_press;

        rRightHandSideVector( index_time ) +=
            R_W( prim );
    }

    KRATOS_CATCH( "" )

}

//************************************************************************************
//CALCULATE EXTERNAL FORCEVECTORS DISPLACEMENT****************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::AddBodyForcesToRHSVectorU( Vector& R, Vector& N_DISP, double density, double Weight, double detJ )
{
    KRATOS_TRY

    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    Vector gravity( dim );

    double density = 0.0;

    if ( GetValue( USE_DISTRIBUTED_PROPERTIES ) )
    {
        noalias( gravity ) = GetValue( GRAVITY );
        density = GetValue( DENSITY );
    }
    else
    {
        noalias( gravity ) = GetProperties()[GRAVITY];
        density = GetProperties()[DENSITY];
    }

    for ( unsigned int prim = 0; prim < ( mNodesDispMax - mNodesDispMin + 1 ); prim++ )
    {
        for ( unsigned int i = 0; i < dim; i++ )
        {
            R( prim*dim + i ) +=
                N_DISP( prim ) * density * gravity( i ) * Weight * detJ;
        }
    }

    KRATOS_CATCH( "" )
}

//************************************************************************************
//CALCULATE INTERNAL FORCEVECTORS DISPLACEMENT****************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::AddInternalForcesToRHSU( Vector& R, const Matrix& B_Operator, Vector& StressVector, double Weight, double detJ )
{
    KRATOS_TRY

    noalias( R ) -= detJ * Weight * prod( trans( B_Operator ), StressVector );

    KRATOS_CATCH( "" )
}

//************************************************************************************
//CALCULATE STIFFNESS MATRICES DISPLACEMENT*******************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateStiffnesMatrixUU( Matrix& K, const
        Matrix& C, const Matrix& B_Operator, const Matrix& DN_DX_DISP, Vector& N_DISP,
        double density, double capillaryPressure, double Weight, double detJ, double waterPressure_scale )
{
    KRATOS_TRY
    unsigned int dim = GetGeometry().WorkingSpaceDimension();
    unsigned int number_of_nodes_disp = mNodesDispMax - mNodesDispMin + 1;

    Vector gravity( dim );

    double density_soil = 0.0;

    if ( GetValue( USE_DISTRIBUTED_PROPERTIES ) )
    {
        noalias( gravity ) = GetValue( GRAVITY );
        density_soil = GetValue( DENSITY );
    }
    else
    {
        noalias( gravity ) = GetProperties()[GRAVITY];
        density_soil = GetProperties()[DENSITY];
    }
    double density_water = GetValue( DENSITY_WATER );
    double density_air = GetValue( DENSITY_AIR );

    double saturation = GetSaturation( capillaryPressure/waterPressure_scale );

    double porosity_divu = 0.0;
//    double porosity_divu= GetDerivativeDPorosityDDivU(DN_DX_DISP);

    double DrhoDdivU =  porosity_divu * ( -density_soil + ( 1 - saturation ) * density_air + saturation * density_water );

    for ( unsigned int prim = 0; prim < number_of_nodes_disp; prim++ )
    {
        for ( unsigned int i = 0; i < dim; i++ )
        {
            for ( unsigned int sec = 0; sec < number_of_nodes_disp; sec++ )
            {
                for ( unsigned int j = 0; j < dim; j++ )
                {
                    K( prim*dim + i, sec*dim + j ) += N_DISP( prim ) * DrhoDdivU * gravity( i ) * DN_DX_DISP( sec, j ) * Weight * detJ;
                }
            }
        }
    }

    noalias( K ) -=
        prod( trans( B_Operator ), ( Weight * detJ ) * Matrix( prod( C, B_Operator ) ) );

    KRATOS_CATCH( "" )
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateStiffnesMatrixUW( Matrix&
        Help_K_UW, Matrix& tanC_W, const Matrix& DN_DX_DISP, Vector& N_DISP, Vector& N_PRESS
        , double capillaryPressure,
        double Weight, double DetJ, double waterPressure_scale )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;

    unsigned int displacement_size = mNodesDispMax - mNodesDispMin + 1;

    Vector gravity( dim );

//    double density_soil = 0.0;

    if ( GetValue( USE_DISTRIBUTED_PROPERTIES ) )
    {
        noalias( gravity ) = GetValue( GRAVITY );
        //density_soil = GetValue( DENSITY );
    }
    else
    {
        noalias( gravity ) = GetProperties()[GRAVITY];
        //density_soil = GetProperties()[DENSITY];
    }
    double density_water = GetValue( DENSITY_WATER );
    double density_air = GetValue( DENSITY_AIR );


    double DSDpc = GetDerivativeDSaturationDpc( capillaryPressure );

    double porosity = GetPorosity( DN_DX_DISP );

    double DrhoDp_w = porosity * ( density_air - density_water ) * DSDpc;


    for ( unsigned int prim = 0; prim < displacement_size; prim++ )
    {
        for ( unsigned int i = 0; i < dim; i++ )
        {
            for ( unsigned int sec = 0; sec < pressure_size; sec++ )
            {
                Help_K_UW( prim*dim + i, sec ) +=
                    N_DISP( prim ) * DrhoDp_w * gravity( i ) * N_PRESS( sec )
                    * Weight * DetJ;

                for ( unsigned int gamma = 0; gamma < 3; gamma++ )
                {
                    Help_K_UW( prim*dim + i, sec ) +=
                        ( -1 ) * ( DN_DX_DISP( prim, gamma ) * tanC_W( i, gamma ) * N_PRESS( sec ) ) * Weight * DetJ*waterPressure_scale;
                }
            }
        }
    }
}

//************************************************************************************
//CALCULATE FORCEVECTORS WATER********************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::AddInternalForcesToRHSW( Vector& Help_R_W, const
        Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double Weight, double  DetJ )
{

    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;

    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    double porosity = GetPorosity( DN_DX_DISP );

    double DS_Dpc = GetDerivativeDSaturationDpc( capillaryPressure );

    double saturation = GetSaturation( capillaryPressure );

    double Dpc_Dt = GetDerivativeDCapillaryPressureDt( N_PRESS );

    double div_Dt = GetDerivativeDDivUDt( DN_DX_DISP );

    Vector flow_water( dim );
    noalias( flow_water ) = GetFlowWater( DN_DX_PRESS, DN_DX_DISP, capillaryPressure );

    for ( unsigned int prim = 0; prim < pressure_size; prim++ )
    {
        Help_R_W( prim ) -=
            N_PRESS( prim ) * porosity * DS_Dpc * Dpc_Dt * Weight * DetJ/* * GetProperties()[SCALE]*/ ;
        Help_R_W( prim ) -=
            N_PRESS( prim ) * saturation * div_Dt * Weight * DetJ/* * GetProperties()[SCALE]*/ ;

        for ( unsigned int gamma = 0; gamma < dim; gamma++ )
        {
            Help_R_W( prim ) +=
                ( DN_DX_PRESS( prim, gamma ) * flow_water( gamma ) )
                * Weight * DetJ/* * GetProperties()[SCALE]*/ ;
        }
    }

}

//************************************************************************************
//CALCULATE STABILISATION TERM FOR FORCEVECTORS WATER*********************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::AddInternalForcesToRHSWs(
    Vector& Help_R_W,
    Vector& N_PRESS,
    Vector& N_PRESS_averaged,
    double capillaryPressure_Dt,
    double averageCapillaryPressure_dt,
    double Weight, double DetJ )
{
  //unsigned int dim = GetGeometry().WorkingSpaceDimension();

    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;

    double t, G;
    if(GetProperties().Has(STABILISATION_FACTOR) && GetProperties().Has(SHEAR_MODULUS))
    {
        t = GetProperties()[STABILISATION_FACTOR];
        G = GetProperties()[SHEAR_MODULUS];
    }
    else
    {
        t = 0.0;
        G = 1.0;
    }
        
    for ( unsigned int prim = 0; prim < pressure_size; ++prim )
    {
        Help_R_W( prim ) += (0.5 * t / G) *
                        ( N_PRESS( prim ) - N_PRESS_averaged( prim ) ) *
                        ( capillaryPressure_Dt - averageCapillaryPressure_dt ) *
                        Weight * DetJ;
    }
}

//************************************************************************************
//CALCULATE STIFFNESS MATRICES WATER**************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateStiffnesMatrixWU
( Matrix& Help_K_WU, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ )
{

    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;

    unsigned int displacement_size = mNodesDispMax - mNodesDispMin + 1;

    double DSDpc = GetDerivativeDSaturationDpc( capillaryPressure );

    double Dpc_Dt = GetDerivativeDCapillaryPressureDt( N_PRESS );

    double DnDdivU = GetDerivativeDPorosityDDivU( DN_DX_DISP );

    Vector flow_water( dim );
    noalias( flow_water ) = GetFlowWater( DN_DX_PRESS, DN_DX_DISP, capillaryPressure );

    for ( unsigned int prim = 0; prim < pressure_size; ++prim )
    {
        for ( unsigned int sec = 0; sec < displacement_size; ++sec )
        {
            for ( unsigned int j = 0; j < dim; ++j )
            {
                Help_K_WU( prim, sec*dim + j ) -=
                    N_PRESS( prim ) * DnDdivU * DSDpc * Dpc_Dt * DN_DX_DISP( sec, j )
                    * Weight * DetJ/* * GetProperties()[SCALE]*/ ;
            }
        }
    }

}

//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateStiffnesMatrixWW
( Matrix& Help_K_WW, const Matrix& DN_DX_DISP, const Matrix& DN_DX_PRESS,
  Vector& N_PRESS, double capillaryPressure, double
  Weight, double DetJ )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;

    double DSDpc = GetDerivativeDSaturationDpc( capillaryPressure );

    double porosity = GetPorosity( DN_DX_DISP );

    double D2S_Dpc2 = GetSecondDerivativeD2SaturationDpc2( capillaryPressure );

    double Dpc_Dt = GetDerivativeDCapillaryPressureDt( N_PRESS );

    Vector Dflow_waterDpw( dim );

    noalias( Dflow_waterDpw ) = GetDerivativeDWaterFlowDpw( DN_DX_PRESS, DN_DX_DISP, capillaryPressure );

    double Dflow_waterDgradpw = GetDerivativeDWaterFlowDGradpw( DN_DX_DISP, capillaryPressure );

    double Ddiv_Dt = GetDerivativeDDivUDt( DN_DX_DISP );

    for ( unsigned int prim = 0; prim < pressure_size; prim++ )
    {
        for ( unsigned int sec = 0; sec < pressure_size; sec++ )
        {
            Help_K_WW( prim, sec ) +=
                N_PRESS( prim ) * porosity * D2S_Dpc2 * Dpc_Dt * N_PRESS( sec )
                * Weight * DetJ/* * GetProperties()[SCALE]*/;

            Help_K_WW( prim, sec ) +=
                N_PRESS( prim ) * DSDpc * Ddiv_Dt * N_PRESS( sec )
                * Weight * DetJ/* * GetProperties()[SCALE]*/;

            for ( unsigned int gamma = 0; gamma < dim; gamma++ )
            {
                Help_K_WW( prim, sec ) +=
                    DN_DX_PRESS( prim, gamma ) * Dflow_waterDpw( gamma )
                    * N_PRESS( sec )
                    * Weight * DetJ/* * GetProperties()[SCALE]*/;
                Help_K_WW( prim, sec ) +=
                    DN_DX_PRESS( prim, gamma ) * Dflow_waterDgradpw
                    * DN_DX_PRESS( sec, gamma ) * Weight * DetJ/* * GetProperties()[SCALE]*/;
            }
        }
    }
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateStiffnesMatrixWWs
( Matrix& Help_K_WW, Vector& N_PRESS, Vector& N_PRESS_averaged, double Weight, double DetJ )
{
}

//************************************************************************************
//CALCULATE DAMPING MATRICES WATER****************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateDampingMatrixWU
( Matrix& Help_D_WU, const Matrix&
  DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;

    unsigned int displacement_size = mNodesDispMax - mNodesDispMin + 1;

    double saturation = GetSaturation( capillaryPressure );

    for ( unsigned int prim = 0; prim < pressure_size; prim++ )
    {
        for ( unsigned int sec = 0; sec < displacement_size; sec++ )
        {
            for ( unsigned int j = 0; j < dim; j++ )
            {
                Help_D_WU( prim, sec*dim + j ) -=
                    N_PRESS( prim ) * saturation * DN_DX_DISP( sec, j )
                    * Weight * DetJ/* * GetProperties()[SCALE]*/;
            }
        }
    }
}

//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateDampingMatrixWW( Matrix& Help_D_WW, const Matrix&
        DN_DX_DISP, Vector& N_PRESS, double capillaryPressure, double Weight, double DetJ )
{
    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;
    double DSDpc = GetDerivativeDSaturationDpc( capillaryPressure );
    double porosity = GetPorosity( DN_DX_DISP );

    for ( unsigned int prim = 0; prim < pressure_size; prim++ )
    {
        for ( unsigned int sec = 0; sec < pressure_size; sec++ )
        {
            Help_D_WW( prim, sec ) +=
                N_PRESS( prim ) * porosity * DSDpc * N_PRESS( sec )
                * Weight * DetJ /* * GetProperties()[SCALE]*/;
        }
    }
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateDampingMatrixWWs( Matrix& Help_D_WW, const Matrix&
        DN_DX_DISP, Vector& N_PRESS, Vector& N_PRESS_averaged, double Weight, double DetJ )
{
  //unsigned int dim = GetGeometry().WorkingSpaceDimension();

    unsigned int pressure_size = mNodesPressMax - mNodesPressMin + 1;

    double t, G;
    if(GetProperties().Has(STABILISATION_FACTOR) && GetProperties().Has(SHEAR_MODULUS))
    {
        t = GetProperties()[STABILISATION_FACTOR];
        G = GetProperties()[SHEAR_MODULUS];
    }
    else
    {
        t = 0.0;
        G = 1.0;
    }
        
    for ( unsigned int prim = 0; prim < pressure_size; ++prim )
    {
        for ( unsigned int sec = 0; sec < pressure_size; ++sec )
        {
            Help_D_WW( prim, sec ) -= (0.5 * t / G) *
                            ( N_PRESS( prim ) - N_PRESS_averaged( prim ) ) *
                            ( N_PRESS( sec ) - N_PRESS_averaged( sec ) ) *
                            Weight * DetJ;
        }
    }
}

//************************************************************************************
//PRIMARY VARIABLES AND THEIR DERIVATIVES
//************************************************************************************
Matrix UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateDisplacementGradient( const Matrix& DN_DX_DISP )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    Matrix result( dim, dim );
    noalias( result ) = ZeroMatrix( dim, dim );

    Vector disp_alpha( dim );

    for ( unsigned int point = ( mNodesDispMin - 1 ); point < mNodesDispMax; point++ )
    {
        noalias( disp_alpha ) = GetGeometry()[point].GetSolutionStepValue( DISPLACEMENT );

        for ( unsigned int j = 0; j < 3; j++ )
        {
            for ( unsigned int k = 0; k < dim; k++ )
            {
                result( j, k ) += disp_alpha( j )
                                  * DN_DX_DISP(( point - mNodesDispMin + 1 ), k );
            }
        }
    }

    return result;
}

//************************************************************************************
//************************************************************************************

Vector UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetGradientWater( const Matrix& DN_DX_PRESS )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    Vector result( dim );
    noalias( result ) = ZeroVector( dim );

    double presW_alpha;

    for ( unsigned int i = mNodesPressMin - 1 ; i < mNodesPressMax ; i++ )
    {
        presW_alpha = GetGeometry()[i].GetSolutionStepValue( WATER_PRESSURE );

        for ( unsigned int k = 0; k < 3; k++ )
        {
            result( k ) +=
                presW_alpha
                * DN_DX_PRESS(( i - mNodesPressMin + 1 ), k );
        }
    }

    return result;
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetPressures( const Vector& N_PRESS,
        double& capillaryPressure, double& waterPressure, const double& waterPressure_scale )
{
    capillaryPressure = 0.0;

    waterPressure = 0.0;

    double presW_alpha;

    for ( unsigned int i = ( mNodesPressMin - 1 ) ; i < mNodesPressMax ; i++ )
    {
        presW_alpha = GetGeometry()[i].GetSolutionStepValue( WATER_PRESSURE ) * waterPressure_scale;

        capillaryPressure +=
            ( -presW_alpha )
            * N_PRESS( i - mNodesPressMin + 1 );

        waterPressure +=
            presW_alpha
            * N_PRESS( i - mNodesPressMin + 1 );

    }
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDerivativeDPressuresDt( const Vector& N_PRESS, double& capillaryPressure_Dt, double& waterPressure_Dt )
{
    capillaryPressure_Dt = 0.0;

    waterPressure_Dt = 0.0;

    double presW_alpha_Dt;

    for ( unsigned int i = mNodesPressMin - 1 ; i < mNodesPressMax ; i++ )
    {
        presW_alpha_Dt =
            GetGeometry()[i].GetSolutionStepValue( WATER_PRESSURE_DT );

        capillaryPressure_Dt +=
            ( -presW_alpha_Dt )
            * N_PRESS( i - mNodesPressMin + 1 );

        waterPressure_Dt +=
            presW_alpha_Dt
            * N_PRESS( i - mNodesPressMin + 1 );
    }
}

//************************************************************************************
//************************************************************************************

double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDerivativeDCapillaryPressureDt( const Vector& N_PRESS )
{
    double capillaryPressure_Dt = 0.0;

    double presW_alpha_Dt;

    for ( unsigned int i = mNodesPressMin - 1 ; i < mNodesPressMax ; i++ )
    {
        presW_alpha_Dt = GetGeometry()[i].GetSolutionStepValue( WATER_PRESSURE_DT );

        capillaryPressure_Dt +=
            ( -presW_alpha_Dt )
            * N_PRESS( i - mNodesPressMin + 1 );

    }

    return capillaryPressure_Dt;
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

//POROSITY AND ITS DERIVATIVES
//TODO check the implementation of GetPorosity, it should depend on the gradient of displacement
double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetPorosity( const Matrix& DN_DX_DISP )
{
    double porosity = GetValue(POROSITY);
    return porosity;
}

double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDerivativeDPorosityDDivU( const Matrix& DN_DX_DISP )
{
//              double initialPorosity= GetProperties()[POROSITY];
//              double div= GetDivU(DN_DX_DISP);

    double porosity_divu = 0.0;
//              double porosity_divu= (1-initialPorosity)*exp(-div);


    return porosity_divu;
}



//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

Vector UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetGravity()
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();
    Vector gravity( dim );
    if( GetValue(USE_DISTRIBUTED_PROPERTIES) )
        noalias( gravity ) = GetValue(GRAVITY);
    else
        noalias( gravity ) = GetProperties()[GRAVITY];
    return gravity;
}

//************************************************************************************
//************************************************************************************
double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDivU( const Matrix& DN_DX_DISP )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    double div = 0.0;

    Vector u_alpha( dim );

    for ( unsigned int i = mNodesDispMin - 1 ; i < mNodesDispMax ; i++ )
    {
        noalias( u_alpha ) =
            GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT );

        for ( unsigned int k = 0; k < dim; k++ )
        {
            div += ( u_alpha( k ) )
                   * DN_DX_DISP( i - mNodesDispMin + 1, k );
        }
    }

    return div;
}

//************************************************************************************
//************************************************************************************
double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDerivativeDDivUDt( const Matrix& DN_DX_DISP )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    double div = 0.0;

    Vector u_alpha_Dt( dim );

    for ( unsigned int i = mNodesDispMin - 1 ; i < mNodesDispMax ; i++ )
    {
        noalias( u_alpha_Dt ) =
            GetGeometry()[i].GetSolutionStepValue( DISPLACEMENT_DT );

        for ( unsigned int k = 0; k < dim; k++ )
        {
            div += u_alpha_Dt( k ) * DN_DX_DISP( i - mNodesDispMin + 1, k );
        }
    }

    return div;
}

//AVERAGED DENSITY
//************************************************************************************
//************************************************************************************

double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetAveragedDensity( double capillaryPressure,
        double porosity )
{
    double result = 0.0;

    double density_soil = 0.0;
    if( GetValue(USE_DISTRIBUTED_PROPERTIES) )
    {
        density_soil = GetValue(DENSITY);
    }
    else
    {
        density_soil = GetProperties()[DENSITY];
    }
    double density_air = GetValue(DENSITY_AIR);
    double density_water = GetValue(DENSITY_WATER);
    double saturation = GetSaturation( capillaryPressure );

    result = ( 1 - porosity ) * density_soil +
             porosity * ( saturation * density_water + ( 1 - saturation ) * density_air );

    return result;
}


//************************************************************************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

//SATURATION AND ITS DERIVATIVES

double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetSaturation( double capillaryPressure )
{
    double airEntryPressure = GetValue(AIR_ENTRY_VALUE);

    if ( airEntryPressure <= 0.0 )
        airEntryPressure = 1.0;

    double b = GetValue(FIRST_SATURATION_PARAM);

    double c = GetValue(SECOND_SATURATION_PARAM);

    double saturation = 0.0;

    if ( capillaryPressure < 0.0 )
        capillaryPressure = 0.0;

    saturation = pow(( 1.0 + pow(( capillaryPressure / airEntryPressure ), b ) ), ( -c ) );

// For Liakopolous Benchmark
// saturation =  1.0-1.9722*1e-11*pow(capillaryPressure,2.4279);

    return saturation;
}

//************************************************************************************
//************************************************************************************

double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDerivativeDSaturationDpc( double capillaryPressure )
{
    double airEntryPressure = GetValue(AIR_ENTRY_VALUE);

    if ( airEntryPressure <= 0 )
        airEntryPressure = 1.0;

    double b = GetValue(FIRST_SATURATION_PARAM);

    double c = GetValue(SECOND_SATURATION_PARAM);

    double result = 0.0;

    if ( capillaryPressure < 0.0 )
    {
        capillaryPressure = 0.0;
    }

    result = ( -c ) * pow(( 1.0 + pow(( capillaryPressure / airEntryPressure ), b ) ), ( -c - 1.0 ) ) * b *
             pow(( capillaryPressure / airEntryPressure ), ( b - 1 ) ) * 1.0 / airEntryPressure;

// For Liakopolous Benchmark
// result =  -1.9722*2.4279*1e-11*pow(capillaryPressure,1.4279);

    return result;
}

//************************************************************************************
//************************************************************************************

double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetSecondDerivativeD2SaturationDpc2( double capillaryPressure )
{
    double airEntryPressure = GetValue(AIR_ENTRY_VALUE);

    if ( airEntryPressure <= 0 )
        airEntryPressure = 1.0;

    double b = GetValue(FIRST_SATURATION_PARAM);

    double c = GetValue(SECOND_SATURATION_PARAM);

    double result = 0.0;

    if ( capillaryPressure < 0.0 )
        capillaryPressure = 0.0;

    result = ( -c ) * b / airEntryPressure * (
                 ( -c - 1.0 ) * pow(( 1 + pow(( capillaryPressure / airEntryPressure ), b ) ), ( -c - 2.0 ) )
                 * b / airEntryPressure * pow(( capillaryPressure / airEntryPressure ), ( 2.0 * ( b - 1.0 ) ) )
                 + pow(( 1.0 + pow(( capillaryPressure / airEntryPressure ), b ) ), ( -c - 1.0 ) ) * ( b - 1.0 )
                 * pow(( capillaryPressure / airEntryPressure ), ( b - 2.0 ) ) * 1.0 / airEntryPressure );

//    double aux1 = capillaryPressure / airEntryPressure;
//    double aux2 = 1.0 + pow(aux1, b );
//    result = ( -c ) * b / airEntryPressure * (
//                   ( -c - 1.0 ) * pow( aux2 , -c - 2.0 ) * b / airEntryPressure * pow(aux1, 2.0 * ( b - 1.0 ) )
//                 + pow(aux2, -c - 1.0 ) * ( b - 1.0 ) * pow(aux1, b - 2.0 ) / airEntryPressure
//                 );

// For Liakopolous Benschmark
// result =  -1.9722*2.4279*1.4279*1e-11*pow(capillaryPressure,0.4279);

    return result;
}

//************************************************************************************
//************************************************************************************

Vector UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetFlowWater( const Matrix& DN_DX_PRESS, const Matrix& DN_DX_DISP, double capillaryPressure )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    //Calculation of relative Permeability after Mualem
    double relPerm = GetSaturation( capillaryPressure );

    if ( relPerm <= 0.01 )
        relPerm = 0.01;

// For Liakopolous Benschmark
// double saturation= GetSaturation(capillaryPressure);
// double relPerm= 1.0- 2.207*pow((1-saturation),1.0121);

    Vector gravity( dim );

    if( GetValue(USE_DISTRIBUTED_PROPERTIES) )
        noalias( gravity ) = GetValue(GRAVITY);
    else
        noalias( gravity ) = GetProperties()[GRAVITY];

    Vector result( dim );

    noalias( result ) = ZeroVector( dim );

    Vector grad_water( dim );

    noalias( grad_water ) = GetGradientWater( DN_DX_PRESS );
    
    for ( unsigned int i = 0; i < dim; i++ )
    {
        result( i ) = -relPerm * GetValue(PERMEABILITY_WATER) /
                      ( GetValue(DENSITY_WATER) * 9.81 )
                      * ( grad_water( i ) - GetValue(DENSITY_WATER)
                          * gravity( i ) );
    }

    return result;
}

//************************************************************************************
//************************************************************************************

Vector UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDerivativeDWaterFlowDpw( const Matrix& DN_DX_PRESS, const Matrix& DN_DX_DISP, double capillaryPressure )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    double DSDpc = GetDerivativeDSaturationDpc( capillaryPressure );
    //Calculation of Derivative relative Permeability after Mualem
    double relPerm = GetSaturation( capillaryPressure );
    double relPerm_pw = ( -1 ) * DSDpc;

    if ( relPerm <= 0.0001 )
    {
        relPerm = 0.0001;
        relPerm_pw = 0.0;
    }

// For Liakopolous Benschmark
// double saturation= GetSaturation(capillaryPressure);
// double relPerm_pw= -2.207*1.0121*pow((1-saturation),0.0121)*(-DSDpc)*(-1);
// double relPerm= 1.0- 2.207*pow((1-saturation),1.0121);

    Vector result( dim );

    noalias( result ) = ZeroVector( dim );

    Vector gravity( dim );

    if( GetValue( USE_DISTRIBUTED_PROPERTIES ) )
        noalias( gravity ) = GetValue(GRAVITY);
    else
        noalias( gravity ) = GetProperties()[GRAVITY];

    Vector grad_water( dim );

    noalias( grad_water ) = GetGradientWater( DN_DX_PRESS );

    for ( unsigned int i = 0; i < dim; i++ )
    {
        result( i ) = -relPerm_pw * GetValue(PERMEABILITY_WATER) /
                      ( GetValue(DENSITY_WATER) * 9.81 )
                      * ( grad_water( i ) - GetValue(DENSITY_WATER)
                          * gravity( i ) );
    }

    return result;
}

//************************************************************************************
//************************************************************************************

double UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetDerivativeDWaterFlowDGradpw( const Matrix& DN_DX_DISP, double capillaryPressure )
{
    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    //Calculation of Derivative relative Permeability after Mualem
    double relPerm = GetSaturation( capillaryPressure );

    if ( relPerm <= 0.0001 )
        relPerm = 0.0001;

// For Liakopolous Benschmark
// double saturation= GetSaturation(capillaryPressure);
// double relPerm= 1.0- 2.207*pow((1-saturation),1.0121);

    double result( dim );

    result =
        -relPerm * GetValue(PERMEABILITY_WATER) / ( GetValue(DENSITY_WATER) * 9.81 );

    return result;
}

//************************************************************************************
//************************************************************************************
//STRESSES, STRAINS AND CONSTITUTIVE MODELL (UNSATURATED CASE)
//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateEffectiveStress( Vector&
        StressVector, Matrix& tanC_W, const
        double waterPressure )
{
    double capillaryPressure = -waterPressure;

    double saturation = GetSaturation( capillaryPressure );

    double DSDpc = GetDerivativeDSaturationDpc( capillaryPressure );

    noalias( tanC_W ) = ZeroMatrix( 3, 3 );

    for ( unsigned int i = 0; i < 3; i++ )
    {
        StressVector( i ) =
            StressVector( i ) - ( saturation * waterPressure );
    }

    for ( unsigned int i = 0; i < 3; i++ )
    {
        tanC_W( i, i ) = ( DSDpc * waterPressure - saturation );
    }
}

//************************************************************************************
//************************************************************************************

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateStressAndTangentialStiffnessUnsaturatedSoils
( Vector& StressVector, Matrix& tanC_U, Matrix& tanC_W,
  Vector& StrainVector, double waterPressure, int PointNumber, const ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    unsigned int dim = GetGeometry().WorkingSpaceDimension();

    if ( tanC_W.size1() != dim || tanC_W.size2() != dim )
        tanC_W.resize( dim, dim );

    noalias( tanC_W ) = ZeroMatrix( dim, dim );

    if ( tanC_U.size1() != 6 || tanC_U.size2() != 6 )
        tanC_U.resize( 6, 6 );

    noalias( tanC_U ) = ZeroMatrix( 6, 6 );

    if ( StressVector.size() != 6 )
        StressVector.resize( 6 );

    double waterPressure_scale = rCurrentProcessInfo[WATER_PRESSURE_SCALE];

    //Set suction in const. law
    mConstitutiveLawVector[PointNumber]->SetValue( SUCTION, -waterPressure/waterPressure_scale,  rCurrentProcessInfo );

    //retrieve the material response
    mConstitutiveLawVector[PointNumber]->CalculateMaterialResponse(
        StrainVector,
        ZeroMatrix( 1 ),
        StressVector,
        tanC_U,
        rCurrentProcessInfo,
        GetProperties(),
        GetGeometry(),
        row( GetGeometry().ShapeFunctionsValues(mThisIntegrationMethod), PointNumber ),
        true,
        1,
        true );

    CalculateEffectiveStress( StressVector, tanC_W, waterPressure/waterPressure_scale );

    KRATOS_CATCH( "" )
}


/**
 * Computes the strain vector
 */
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateStrain( const Matrix& B, const Matrix& Displacements, Vector& StrainVector )
{
    KRATOS_TRY
    noalias( StrainVector ) = ZeroVector( 6 );

    for ( unsigned int node = 0; node < GetGeometry().size(); node++ )
    {
        for ( unsigned int item = 0; item < 6; item++ )
            for ( unsigned int dim = 0; dim < 3; dim++ )
                StrainVector[item] += B( item, 3 * node + dim ) * ( Displacements( node, dim ) - mInitialDisp( node, dim ) );
    }

    KRATOS_CATCH( "" )
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateBoperator
( Matrix& B_Operator, const Matrix& DN_DX )
{
    KRATOS_TRY

    unsigned int number_of_nodes_disp = ( mNodesDispMax - mNodesDispMin + 1 );

    noalias( B_Operator ) = ZeroMatrix( 6, number_of_nodes_disp * 3 );

    for ( unsigned int i = 0; i < number_of_nodes_disp; ++i )
    {
        B_Operator( 0, i*3 )     = DN_DX( i, 0 );
        B_Operator( 1, i*3 + 1 ) = DN_DX( i, 1 );
        B_Operator( 2, i*3 + 2 ) = DN_DX( i, 2 );
        B_Operator( 3, i*3 )     = DN_DX( i, 1 );
        B_Operator( 3, i*3 + 1 ) = DN_DX( i, 0 );
        B_Operator( 4, i*3 + 1 ) = DN_DX( i, 2 );
        B_Operator( 4, i*3 + 2 ) = DN_DX( i, 1 );
        B_Operator( 5, i*3 )     = DN_DX( i, 2 );
        B_Operator( 5, i*3 + 2 ) = DN_DX( i, 0 );
    }

    KRATOS_CATCH( "" )
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::CalculateBBaroperator
( Matrix& B_Operator, const Matrix& DN_DX, const Matrix& Bdil_bar )
{
    KRATOS_TRY

    unsigned int number_of_nodes_disp = ( mNodesDispMax - mNodesDispMin + 1 );
    
    noalias( B_Operator ) = ZeroMatrix( 6, number_of_nodes_disp * 3 );

    double aux = (1.0 / 3);
    double tmp1;
    double tmp2;
    double tmp3;
    for ( unsigned int i = 0; i < number_of_nodes_disp; ++i )
    {
        tmp1 = aux * (Bdil_bar( i, 0 ) - DN_DX( i, 0 ));
        tmp2 = aux * (Bdil_bar( i, 1 ) - DN_DX( i, 1 ));
        tmp3 = aux * (Bdil_bar( i, 2 ) - DN_DX( i, 2 ));
        
        B_Operator( 0, i*3 ) = DN_DX( i, 0 ) + tmp1;
        B_Operator( 1, i*3 ) = tmp1;
        B_Operator( 2, i*3 ) = tmp1;

        B_Operator( 0, i*3 + 1)  = tmp2;
        B_Operator( 1, i*3 + 1 ) = DN_DX( i, 1 ) + tmp2;
        B_Operator( 2, i*3 + 1 ) = tmp2;
        
        B_Operator( 0, i*3 + 2)  = tmp3;
        B_Operator( 1, i*3 + 2 ) = tmp3;
        B_Operator( 2, i*3 + 2 ) = DN_DX( i, 2 ) + tmp3;
        
        B_Operator( 3, i*3 )     = DN_DX( i, 1 );
        B_Operator( 3, i*3 + 1 ) = DN_DX( i, 0 );
        B_Operator( 4, i*3 + 1 ) = DN_DX( i, 2 );
        B_Operator( 4, i*3 + 2 ) = DN_DX( i, 1 );
        B_Operator( 5, i*3 )     = DN_DX( i, 2 );
        B_Operator( 5, i*3 + 2 ) = DN_DX( i, 0 );
    }
    
    KRATOS_CATCH( "" )
}

//************************************************************************************
//************************************************************************************
void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::InitializeMaterial
()
{
    KRATOS_TRY

    #ifdef ENABLE_BEZIER_GEOMETRY
    GetGeometry().Initialize(mThisIntegrationMethod);
    #endif

    for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); ++i )
    {
        mConstitutiveLawVector[i] = GetProperties()[CONSTITUTIVE_LAW]->Clone();
        mConstitutiveLawVector[i]->SetValue( PARENT_ELEMENT_ID, this->Id(), *(ProcessInfo*)0);
        mConstitutiveLawVector[i]->SetValue( INTEGRATION_POINT_INDEX, i, *(ProcessInfo*)0);
        mConstitutiveLawVector[i]->InitializeMaterial( GetProperties(), GetGeometry(), row( GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod ), i ) );
    }

    #ifdef ENABLE_BEZIER_GEOMETRY
    GetGeometry().Clean();
    #endif

    KRATOS_CATCH( "" )
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetValueOnIntegrationPoints( const Variable<Matrix>& rVariable, std::vector<Matrix>& rValues, const ProcessInfo& rCurrentProcessInfo )
{
    if ( rValues.size() != mConstitutiveLawVector.size() )
        rValues.resize( mConstitutiveLawVector.size() );

    if ( rVariable == ELASTIC_LEFT_CAUCHY_GREEN_OLD )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            if ( rValues[i].size1() != 3 || rValues[i].size2() != 3 )
                rValues[i].resize( 3, 3 );

            noalias( rValues[i] ) = mConstitutiveLawVector[i]->GetValue( ELASTIC_LEFT_CAUCHY_GREEN_OLD, rValues[i] );
        }
    }
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetValueOnIntegrationPoints( const Variable<Vector>& rVariable, std::vector<Vector>& rValues, const ProcessInfo& rCurrentProcessInfo )
{
    const GeometryType::IntegrationPointsArrayType& integration_points = GetGeometry().IntegrationPoints( mThisIntegrationMethod );

    if ( rValues.size() != mConstitutiveLawVector.size() )
        rValues.resize( mConstitutiveLawVector.size() );

    if ( rVariable == MATERIAL_PARAMETERS )
    {
        if ( rValues.size() != mConstitutiveLawVector.size() )
            rValues.resize( mConstitutiveLawVector.size() );

        for ( unsigned int ii = 0; ii < mConstitutiveLawVector.size(); ii++ )
            rValues[ii] = mConstitutiveLawVector[ii]->GetValue( rVariable, rValues[ii] );
    }

    if ( rVariable == INSITU_STRESS || rVariable == PRESTRESS )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            if ( rValues[i].size() != 6 )
                rValues[i].resize( 6 );

            noalias( rValues[i] ) = mConstitutiveLawVector[i]->GetValue( PRESTRESS, rValues[i] );
        }
    }
    
    if ( rVariable == PLASTIC_STRAIN_VECTOR )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            if ( rValues[i].size() != 6 )
                rValues[i].resize( 6 );

            noalias( rValues[i] ) = mConstitutiveLawVector[i]->GetValue( PLASTIC_STRAIN_VECTOR, rValues[i] );
        }
    }

    //To Plot Internal variables
    if ( rVariable == INTERNAL_VARIABLES )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            if ( rValues[i].size() != 9 )
                rValues[i].resize( 9 );

            noalias( rValues[i] ) = mConstitutiveLawVector[i]->GetValue( INTERNAL_VARIABLES, rValues[i] );
            
        }
    }

    //To Plot Stresses
    if ( rVariable == STRESSES )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            if ( rValues[i].size() != 6 )
                rValues[i].resize( 6 );

            noalias( rValues[i] ) = mConstitutiveLawVector[i]->GetValue( STRESSES, rValues[i] );

        }
    }

    //To Plot Fluid Flows
    if ( rVariable == FLUID_FLOWS )
    {
        unsigned int number_of_nodes_press = ( mNodesPressMax - mNodesPressMin + 1 );

        Vector N_PRESS( number_of_nodes_press );

        Matrix DN_DX_PRESS( number_of_nodes_press, 3 );
        Matrix DN_DX_DISP( 0, 0 );

        double capillaryPressure;

        double waterPressure;

        double saturation;

        Vector waterFlow( 3 );

        #ifdef ENABLE_BEZIER_GEOMETRY
        GetGeometry().Initialize(mThisIntegrationMethod);
        #endif

        const GeometryType::ShapeFunctionsGradientsType& DN_De_Displacement =
            GetGeometry().ShapeFunctionsLocalGradients( mThisIntegrationMethod );
        const GeometryType::ShapeFunctionsGradientsType& DN_De_Pressure =
            mpPressureGeometry->ShapeFunctionsLocalGradients( mThisIntegrationMethod );

        const Matrix& Ncontainer_Pressure = mpPressureGeometry->ShapeFunctionsValues( mThisIntegrationMethod );

        for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); PointNumber++ )
        {
            if ( rValues[PointNumber].size() != 9 )
                rValues[PointNumber].resize( 9 );

            // Shape Functions on current spatial quadrature point
            if ( N_PRESS.size() != number_of_nodes_press )
                N_PRESS.resize( number_of_nodes_press );

            noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );

            GetPressures( N_PRESS, capillaryPressure, waterPressure, rCurrentProcessInfo[WATER_PRESSURE_SCALE] );

            saturation = GetSaturation( capillaryPressure );

            rValues[PointNumber][0] = waterPressure;

            rValues[PointNumber][1] = 0.0;

            rValues[PointNumber][2] = saturation;

            noalias( DN_DX_PRESS ) = prod( DN_De_Pressure[PointNumber], mInvJ0[PointNumber] );

            noalias( DN_DX_DISP ) = prod( DN_De_Displacement[PointNumber], mInvJ0[PointNumber] );

            noalias( waterFlow ) = GetFlowWater( DN_DX_PRESS, DN_DX_DISP, capillaryPressure );

            rValues[PointNumber][3] = waterFlow( 0 );

            rValues[PointNumber][4] = waterFlow( 1 );

            rValues[PointNumber][5] = waterFlow( 2 );

            rValues[PointNumber][6] = 0.0;

            rValues[PointNumber][7] = 0.0;

            rValues[PointNumber][8] = 0.0;
        }

        #ifdef ENABLE_BEZIER_GEOMETRY
        GetGeometry().Clean();
        #endif
    }

    //To Plot Coordinates of Integration Points
    if ( rVariable == COORDINATES )
    {
        for ( unsigned int i = 0; i < integration_points.size(); i++ )
        {
            if ( rValues[i].size() != 3 )
                rValues[i].resize( 3 );

            Geometry<Node<3> >::CoordinatesArrayType dummy;

            GetGeometry().GlobalCoordinates( dummy, integration_points[i] );

            noalias( rValues[i] ) = dummy;
        }
    }

    if ( rVariable == STRAIN || rVariable == CURRENT_STRAIN_VECTOR )
    {
        #ifdef ENABLE_BEZIER_GEOMETRY
        //initialize the geometry
        GetGeometry().Initialize(mThisIntegrationMethod);
        #endif

        // calculate shape function values and local gradients
        unsigned int dim = GetGeometry().WorkingSpaceDimension();
        unsigned int number_of_nodes = GetGeometry().size();
        unsigned int strain_size = dim * (dim + 1) / 2;
        unsigned int mat_size = dim * number_of_nodes;
        Matrix B(strain_size, mat_size);
        Vector StrainVector(strain_size);
        Matrix DN_DX(number_of_nodes, dim);
        Matrix CurrentDisp(number_of_nodes, dim);

        const GeometryType::ShapeFunctionsGradientsType& DN_De = GetGeometry().ShapeFunctionsLocalGradients( mThisIntegrationMethod );
//        const Matrix& Ncontainer = GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod );

        // extract current displacements
        for (unsigned int node = 0; node < GetGeometry().size(); ++node)
            noalias(row(CurrentDisp, node)) =
                GetGeometry()[node].GetSolutionStepValue(DISPLACEMENT);

        for (unsigned int i = 0; i < mConstitutiveLawVector.size(); ++i)
        {
            if (rValues[i].size() != 6)
                rValues[i].resize(6);

            // compute B_Operator at the current integration point
            noalias(DN_DX) = prod(DN_De[i], mInvJ0[i]);
            CalculateBoperator(B, DN_DX);

            // compute the strain at integration point
            CalculateStrain(B, CurrentDisp, StrainVector);
            if(dim == 2)
            {
                rValues[i](0) = StrainVector(0);
                rValues[i](1) = StrainVector(1);
                rValues[i](2) = 0.0; // note: it's only correct for plane strain, TODO: we must make this available for plane stress constitutive law
                rValues[i](3) = StrainVector(2);
                rValues[i](4) = 0.0;
                rValues[i](5) = 0.0;
            }
            else if(dim == 3)
                noalias(rValues[i]) = StrainVector;
        }

        #ifdef ENABLE_BEZIER_GEOMETRY
        GetGeometry().Clean();
        #endif
    }
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetValueOnIntegrationPoints( const Variable<double>& rVariable, std::vector<double>& rValues, const ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY
    
    if( rVariable == PLASTICITY_INDICATOR )
    {
        if ( rValues.size() != mConstitutiveLawVector.size() )
            rValues.resize( mConstitutiveLawVector.size() );

        //reading integration points and local gradients
        for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); Point++ )
        {
            rValues[Point] = mConstitutiveLawVector[Point]->GetValue( rVariable, rValues[Point] );
        }
        return;
    }
    
    if( rVariable == K0 )
    {
        if ( rValues.size() != mConstitutiveLawVector.size() )
            rValues.resize( mConstitutiveLawVector.size() );
        
        for ( unsigned int Point = 0; Point < mConstitutiveLawVector.size(); Point++ )
        {
            rValues[Point] = GetValue( K0 );
        }
        
        return;
    }

    unsigned int number_of_nodes_press = ( mNodesPressMax - mNodesPressMin + 1 );

    #ifdef ENABLE_BEZIER_GEOMETRY
    GetGeometry().Initialize(mThisIntegrationMethod);
    #endif

    //reading integration points and local gradients
    const GeometryType::IntegrationPointsArrayType& integration_points = GetGeometry().IntegrationPoints( mThisIntegrationMethod );

    if ( rValues.size() != integration_points.size() )
        rValues.resize( integration_points.size() );

    const Matrix& Ncontainer_Pressure = mpPressureGeometry->ShapeFunctionsValues( mThisIntegrationMethod );

    Vector N_PRESS( number_of_nodes_press );

    double capillaryPressure;

    double waterPressure;

    double saturation;


    /////////////////////////////////////////////////////////////////////////
    //// Integration in space sum_(beta=0)^(number of quadrature points)
    /////////////////////////////////////////////////////////////////////////
    for ( unsigned int PointNumber = 0; PointNumber < integration_points.size(); PointNumber++ )
    {
        GeometryType::CoordinatesArrayType gp_position;
        gp_position = GetGeometry().GlobalCoordinates( gp_position, integration_points[PointNumber] );

        // Shape Functions on current spatial quadrature point
        if ( N_PRESS.size() != number_of_nodes_press )
            N_PRESS.resize( number_of_nodes_press );

        noalias( N_PRESS ) = row( Ncontainer_Pressure, PointNumber );

        GetPressures( N_PRESS, capillaryPressure, waterPressure, rCurrentProcessInfo[WATER_PRESSURE_SCALE] );

        saturation = GetSaturation( capillaryPressure );

        if ( rVariable == SATURATION )
        {
            rValues[PointNumber] = saturation;
        }

        if ( rVariable == WATER_PRESSURE )
        {
            rValues[PointNumber] = waterPressure;
//            KRATOS_WATCH(waterPressure)
        }

        if ( rVariable == EXCESS_PORE_WATER_PRESSURE )
        {
            rValues[PointNumber] = waterPressure - mReferencePressures[PointNumber];
        }
    }
    /////////////////////////////////////////////////////////////////////////
    //// End Integration in space sum_(beta=0)^(number of quadrature points)
    /////////////////////////////////////////////////////////////////////////

    #ifdef ENABLE_BEZIER_GEOMETRY
    GetGeometry().Clean();
    #endif

    KRATOS_CATCH( "" )
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::SetValueOnIntegrationPoints( const Variable<Matrix>& rVariable, std::vector<Matrix>& rValues, const ProcessInfo& rCurrentProcessInfo )
{
    if ( rValues.size() != mConstitutiveLawVector.size() )
    {
        std::cout << "wrong size: " << rValues.size() << "!=" << mConstitutiveLawVector.size() << std::endl;
        return;
    }

    if ( rVariable == ELASTIC_LEFT_CAUCHY_GREEN_OLD )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            mConstitutiveLawVector[i]->SetValue( ELASTIC_LEFT_CAUCHY_GREEN_OLD, rValues[i], rCurrentProcessInfo );
        }
    }
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::SetValueOnIntegrationPoints( const Variable<Vector>& rVariable, std::vector<Vector>& rValues, const ProcessInfo& rCurrentProcessInfo )
{
    if ( rValues.size() != mConstitutiveLawVector.size() )
        return;

    if ( rVariable == INSITU_STRESS || rVariable == PRESTRESS )
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
            mConstitutiveLawVector[i]->SetValue( PRESTRESS, rValues[i], rCurrentProcessInfo );

    if ( rVariable == INTERNAL_VARIABLES )
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
            mConstitutiveLawVector[i]->SetValue( INTERNAL_VARIABLES, rValues[i], rCurrentProcessInfo );

    if ( rVariable == MATERIAL_PARAMETERS )
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
            mConstitutiveLawVector[i]->SetValue( rVariable, rValues[i], rCurrentProcessInfo );
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::SetValueOnIntegrationPoints( const Kratos::Variable< ConstitutiveLaw::Pointer >& rVariable, std::vector< ConstitutiveLaw::Pointer >& rValues, const Kratos::ProcessInfo& rCurrentProcessInfo )
{
    if ( rVariable == CONSTITUTIVE_LAW )
    {
        for ( unsigned int i = 0; i < rValues.size(); i++ )
        {
            mConstitutiveLawVector[i] = rValues[i];
            mConstitutiveLawVector[i]->InitializeMaterial( GetProperties(), GetGeometry(), row( GetGeometry().ShapeFunctionsValues( mThisIntegrationMethod ), i ) );
        }
    }
}

void UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::SetValueOnIntegrationPoints( const Variable<double>& rVariable, std::vector<double>& rValues, const ProcessInfo& rCurrentProcessInfo )
{
    if( rVariable == K0 )
    {
        SetValue( K0, rValues[0] );
    }
    
    else if( rVariable == REFERENCE_WATER_PRESSURE )
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            mReferencePressures[i] = rValues[i];
        }
    }
    else
    {
        for ( unsigned int i = 0; i < mConstitutiveLawVector.size(); i++ )
        {
            mConstitutiveLawVector[i]->SetValue( rVariable, rValues[i], rCurrentProcessInfo );
        }
    }

}

UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::IntegrationMethod UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::GetIntegrationMethod() const
{
    return mThisIntegrationMethod;
}

int UnsaturatedSoilsElement_2phase_SmallStrain_Scale_WaterPressure::Check(const Kratos::ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    if (this->Id() < 1)
    {
        KRATOS_THROW_ERROR(std::logic_error, "Element found with Id 0 or negative", __FUNCTION__);
    }

    if (mTotalDomainInitialSize < 0)
    {
        std::cout << "error on element -> " << this->Id() << std::endl;
        std::cout << "mTotalDomainInitialSize = " << mTotalDomainInitialSize << std::endl;
        KRATOS_THROW_ERROR(std::logic_error, "Domain size can not be less than 0. Please check Jacobian.", __FUNCTION__);
    }

    //verify that the constitutive law exists
    if (this->GetProperties().Has(CONSTITUTIVE_LAW) == false)
    {
        KRATOS_THROW_ERROR(std::logic_error, "constitutive law not provided for property ", this->GetProperties().Id());
    }

    KRATOS_CATCH( "" )

    return 1;
}

} // Namespace Kratos

#undef ENABLE_DEBUG_CONSTITUTIVE_LAW