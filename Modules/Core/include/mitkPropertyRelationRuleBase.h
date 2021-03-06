/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef mitkPropertyRelationRuleBase_h
#define mitkPropertyRelationRuleBase_h

#include "mitkIPropertyOwner.h"
#include "mitkIdentifiable.h"

#include "mitkException.h"
#include "mitkNodePredicateBase.h"
#include "mitkPropertyKeyPath.h"

#include <MitkCoreExports.h>

#include <string>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace mitk
{
  /**Base class to standardize/abstract/encapsulate rules and business logic to detect and define
  (property/data based) relations in MITK.
  Following important definitions must be regarded when using/implementing/specifing rule classes:
  - Releations represented by rules are directed relations that point from a source IPropertyOwner (Source)
  to a destination IPropertyOwner (Destination).
  - A concrete rule ID always "implements" a concrete relation type. E.g. In DICOM the
  way to express the source image relation to an input image and to a mask would be nearly the same
  and only differs by the encoded purpuse. One may implement an interim or joined class that manages the mutual
  stuff, but the registered instances must be one for "DICOM source input image" and one for
  "DICOM source mask" and have distinct rule IDs.
  - Source may have several relations of a rule to different Destinations.
  Destination may have several relations of a rule from different Sources. But a specific Source Destination
  pair may have only one relation of a specific rule.
  - The deletion of a Destination in the storage does not remove the relation implicitly. It becomes a "zombie" relation
  but it should still be documented, even if the destination is unknown. One has to explicitly
  disconnect a zombie relation to get rid of it.
  - Each relation has its own UID (relationUID) that can be used to address it.

  The basic concept of the rule design is that we have two layers of relation identification: Layer 1 is the ID-layer
  which uses the IIdentifiable interface and UIDs if available to encode "hard" relations. Layer 2 is the Data-layer
  which uses the properties of Source and Destination to deduce if there is a relation of the rule type.
  The ID-layer is completely implemented by this base class. The base class only falls back to the Data-layer
  (implemented by the concrete rule class) if the ID-layer is not sufficient or it is explicitly stated.
  Reasons for the introduction of the ID-layer are: 1st, data-defined relations may be weak (several Destinations are
  possible; e.g. DICOM source images may point to several loaded mitk images). But if explicitly a relation was
  connected it should be deduceable. 2nd, checks on a UID are faster then unnecessary data deduction.

  Rules use relation instance identifing (RII) properties in order to manage their relations that are stored in the
  Source. The RII-properties follow the following naming schema:
  "MITK.Relations.<RuleID>.<InstanceID>.[relationUID|destinationUID|<data-layer-specific>]"
  - <RuleID>: The identifier of the concrete rule that sets the property
  - <InstanceID>: The unique index of the relation of the rule for the Source. Used to assign/group the properties to
  their relation. In the default implementation of this class the instance id is an positive integer (i>0).
  - relationUID: The UID of the relation. Set by the ID-layer (so by this class)
  - destinationUID: The UID of the Destination. Set by the ID-layer (so by this class) if Destination implements
  IIdentifiable.
  - <data-layer-specific>: Information needed by the Data-layer (so derived classes) to find the relationUID
  */
  class MITKCORE_EXPORT PropertyRelationRuleBase : public itk::Object
  {
  public:
    mitkClassMacroItkParent(PropertyRelationRuleBase, itk::Object);
    itkCloneMacro(Self);
    itkCreateAnotherMacro(Self);

    using RuleIDType = std::string;
    using RelationUIDType = Identifiable::UIDType;
    using RelationUIDVectorType = std::vector<RelationUIDType>;

    /** Enum class for different types of relations. */
    enum class RelationType
    {
      None = 0,           /**< Two IPropertyOwner have no relation under the rule.*/
      Implicit_Data = 1,  /**< Two IPropertyOwner have a relation, but it is only deduced from the Data-layer and they
                   were never explicitly connected.*/
      Connected_Data = 2, /**< Two IPropertyOwner have a relation and there where connected on the Data-layer (so a bit
                   "weaker" as ID). Reasons for the missing ID connection could be that Destintination has not
                   IIdentifiable implemented.*/
      Connected_ID = 4    /**< Two IPropertyOwner have a relation and are fully explictly connected.*/
    };

    /** Returns an ID string that identifies the rule class */
    virtual RuleIDType GetRuleID() const = 0;

    /** Returns a human readable string that can be used to describe the rule. Does not need to be unique.*/
    virtual std::string GetDisplayName() const = 0;

    /** Returns a human readable string that can be used to describe the role of a source in context of the rule
     * instance.*/
    virtual std::string GetSourceRoleName() const = 0;
    /** Returns a human readable string that can be used to describe the role of a destionation in context of the rule
     * instance.*/
    virtual std::string GetDestinationRoleName() const = 0;

    /** This method checks if owner is eligible to be a Source for the rule. The default implementation returns a
    True for every valid IPropertyProvider (so only a null_ptr results into false). May be reimplement by derived rules if
    they have requirements on potential Sources).*/
    virtual bool IsSourceCandidate(const IPropertyProvider *owner) const;

    /** This method checks if owner is eligible to be a Destination for the rule. The default implementation returns a
    True for every valid IPropertyProvider (so only a null_ptr results into false). May be reimplement by derived rules if
    they have requirements on potential Sources).*/
    virtual bool IsDestinationCandidate(const IPropertyProvider *owner) const;

    /** Returns true if the passed owner is a Source of a relation defined by the rule so has at least one relation of
    type Connected_Data or Connected_ID.
    @pre owner must be a pointer to a valid IPropertyProvider instance.*/
    bool IsSource(const IPropertyProvider *owner) const;

    /** Returns the relation type of the passed IPropertyOwner instances.
    @pre source must be a pointer to a valid IPropertyProvider instance.
    @pre destination must be a pointer to a valid IPropertyProvider instance.
    */
    RelationType HasRelation(const IPropertyProvider *source, const IPropertyProvider *destination) const;

    /** Returns a vector of relation UIDs for all relations of this rule instance that are defined for
    the passed source (so relations of type Connected_Data and Connected_ID).
    @pre source must be a pointer to a valid IPropertyOwner instance.
    */
    RelationUIDVectorType GetExistingRelations(const IPropertyProvider *source) const;

    /** Returns the relation UID for the passed source and destination of this rule instance.
    If the passed instances have no explicit relation (so of type Connected_Data or Connected_ID),
    no ID can be deduced and an exception will be thrown.
    @pre source must be a pointer to a valid IPropertyOwner instance.
    @pre destination must be a pointer to a valid IPropertyOwner instance.
    @pre Source and destination have a relation of type Connected_Data or Connected_ID; otherwise
    NoPropertyRelationException is thrown.*/
    RelationUIDType GetRelationUID(const IPropertyProvider *source, const IPropertyProvider *destination) const;

    /**Predicate that can be used to find nodes that qualify as source for that rule (but must not be a source yet).
    Thus all nodes where IsSourceCandidate() returns true. */
    NodePredicateBase::ConstPointer GetSourceCandidateIndicator() const;
    /**Predicate that can be used to find nodes that qualify as destination for that rule (but must not be a destination
    yet). Thus all nodes where IsDestinationCandidate() returns true. */
    NodePredicateBase::ConstPointer GetDestinationCandidateIndicator() const;
    /**Predicate that can be used to find nodes that are Sources of that rule and explicitly connected.
    Thus all nodes where IsSource() returns true.*/
    NodePredicateBase::ConstPointer GetConnectedSourcesDetector() const;
    /**Predicate that can be used to find nodes that are as source related to the passed Destination under the rule
    @param destination Pointer to the Destination instance that should be used for detection.
    @param minimalRelation Defines the minimal strength of the relation type that should be detected.
    @pre Destination must be a valid instance.*/
    NodePredicateBase::ConstPointer GetSourcesDetector(
      const IPropertyProvider *destination, RelationType minimalRelation = RelationType::Implicit_Data) const;
    /**Predicate that can be used to find nodes that are as Destination related to the passed Source under the rule
    @param source Pointer to the Source instance that should be used for detection.
    @param minimalRelation Defines the minimal strength of the relation type that should be detected.
    @pre Destination must be a valid instance.*/
    NodePredicateBase::ConstPointer GetDestinationsDetector(
      const IPropertyProvider *source, RelationType minimalRelation = RelationType::Implicit_Data) const;
    /**Returns a predicate that can be used to find the Destination of the passed Source for a given relationUID.
    @param source Pointer to the Source instance that should be used for detection.
    @param minimalRelation Defines the minimal strength of the relation type that should be detected.
    @pre source must be a valid instance.
    @pre relationUID must identify a relation of the passed source and rule. (This must be in the return of
    this->GetExistingRelations(source). */
    NodePredicateBase::ConstPointer GetDestinationDetector(const IPropertyProvider *source,
                                                           RelationUIDType relationUID) const;

    /**Explicitly connects the passed instances. Afterwards they have a relation of Connected_Data or Connected_ID (if a
    destination implements IIdentifiable). If the passed instance are already connected the old connection will be
    overwritten (and raised to the highest possible connection level).
    @pre source must be a valid instance.
    @pre destination must be a valid instance.*/
    void Connect(IPropertyOwner *source, const IPropertyProvider *destination) const;
    /**Disconnect the passed instances. Afterwards they have a relation of None or Implicit_Data.
    All RII-properties in the source for the passed destination will be removed.
    @pre source must be a valid instance.
    @pre destination must be a valid instance.*/
    void Disconnect(IPropertyOwner *source, const IPropertyProvider *destination) const;
    /**Disconnect the source from the passed relationUID (usefull for "zombie relations").
    All RII-properties in the source for the passed relationUID will be removed.
    If the relationUID is not part of the source. Nothing will be changed.
    @pre source must be a valid instance.*/
    void Disconnect(IPropertyOwner *source, RelationUIDType relationUID) const;

  protected:
    PropertyRelationRuleBase() = default;
    ~PropertyRelationRuleBase() override = default;

    using InstanceIDType = std::string;
    using InstanceIDVectorType = std::vector<InstanceIDType>;
    static InstanceIDType NULL_INSTANCE_ID();

    /** Returns the instance ID for the passed source and destination for this rule instance.
    If the passed source and destination instances has no explicit relation on the ID layer (Connected_ID),
    NULL_INSTANCE_ID will be returned to imply the non existing relation.
    @pre source must be a pointer to a valid IPropertyProvider instance.
    @pre destination must be a pointer to a valid IPropertyProvider instance.*/
    InstanceIDType GetInstanceID_IDLayer(const IPropertyProvider *source,
                                                                         const IPropertyProvider *destination) const;

    /** Is called if an instance ID cannot be deduced on the ID-layer.
    Implement this method to check which existing relation(s) as Connected_Data exists between
    both passed instances. If the passed instances have no
    explicit relation of type Connected_Data, an empty vector will be returned.
    @remark Per definition of property relation rules only 0 or 1 instance should be found for one provider
    pair and rule. But the data layer may be ambiguous and therefore multiple relation instances of the rule instance
    could match. The implementation of this function should report all relation instances. The calling function
    will take care of this violation.
    @pre source must be a pointer to a valid IPropertyProvider instance.
    @pre destination must be a pointer to a valid IPropertyProvider instance.*/
    virtual InstanceIDVectorType GetInstanceID_datalayer(const IPropertyProvider *source,
                                                         const IPropertyProvider *destination) const = 0;

    /** Is called by HasRelation() if no relation of type Connected_ID (GetInstanceID_IDLayer()) or
    Connected_Data (GetInstanceID_datalayer()) is evident.
      Implement this method to deduce if the passed instances have a relation of type
      Implicit_Data.
      @pre source must be a pointer to a valid IPropertyProvider instance.
      @pre destination must be a pointer to a valid IPropertyProvider instance.
      */
    virtual bool HasImplicitDataRelation(const IPropertyProvider *source,
                                         const IPropertyProvider *destination) const = 0;

    /**Helper function that deduces the relation UID of the given relation instance.
    If it cannot be deduced an NoPropertyRelationException is thrown.*/
    RelationUIDType GetRelationUIDByInstanceID(const IPropertyProvider *source, const InstanceIDType &instanceID) const;
    /**Helper function that deduces the relation instance ID given the relation UID.
    If it cannot be deduced an NoPropertyRelationException is thrown.*/
    InstanceIDType GetInstanceIDByRelationUID(const IPropertyProvider *source,
                                              const RelationUIDType &relationUID) const;

    /**Is called by Connect() to ensure that source has correctly set properties to resemble
    the relation on the data layer. This means that the method should set the properties that describe
    and encode the relation on the data layer (data-layer-specific relation properties).
    If the passed instance are already connected, the old settings should be
    overwritten. Connect() will ensure that source and destination are valid pointers.
    @param instanceID is the ID for the relation instance that should be connected. Existance of the relation instance
    is ensured.
    @pre source must be a valid instance.
    @pre destination must be a valid instance.*/
    virtual void Connect_datalayer(IPropertyOwner *source,
                                   const IPropertyProvider *destination,
                                   const InstanceIDType &instanceID) const = 0;

    /**This method is called by Disconnect() to remove all properties of the relation from the source that
    are set by Connect_datalayer().
    @remark All RII-properties of this relation will removed by Disconnect() after this method call.
    If the relationUID is not part of the source. Nothing will be changed. Disconnect() ensures that source is a valid
    pointer if called.
    @remark Disconnect() ensures that sourece is valid and only invokes if instance exists.*/
    virtual void Disconnect_datalayer(IPropertyOwner *source, const InstanceIDType &instanceID) const = 0;

    /** Returns the generic root path for relation rules ("MITK.Relations").*/
    static PropertyKeyPath GetRootKeyPath();

    /** Returns the root path for the rule instance ("MITK.Relations.<RuleID>"). */
    PropertyKeyPath GetRuleRootKeyPath() const;

    itk::LightObject::Pointer InternalClone() const override;

  private:
    /** Creats a relation UID*/
    static RelationUIDType CreateRelationUID();

    /**Prepares a new relation instance. Therefore an unused and valid instance ID for the passed source and the rule
    instance will be genarated and a relationUID property with the relationUID will be set to block the instance ID. The
    instance ID will be returned.
    @remark The method is guarded by a class wide mutex to avoid racing conditions in a scenario where rules are used
    concurrently.*/
    InstanceIDType CreateNewRelationInstance(IPropertyOwner *source, const RelationUIDType &relationUID) const;
  };

  /**Exception that is used by PropertyRelationRuleBase based classes to indicate that two objects have no relation.*/
  class NoPropertyRelationException : public Exception
  {
  public:
    mitkExceptionClassMacro(NoPropertyRelationException, Exception)
  };

} // namespace mitk

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
